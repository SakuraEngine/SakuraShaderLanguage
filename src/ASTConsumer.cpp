#include "ASTConsumer.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/DeclTemplate.h"
#include "Attributes/ShaderAttr.h"
#include "clang/AST/Attr.h"

#include <fstream>

void ssl::ASTConsumer::HandleTranslationUnit(ASTContext& ctx)
{
    // 1. collect
    _ASTContext = &ctx;
    auto tuDecl = ctx.getTranslationUnitDecl();
    for (clang::DeclContext::decl_iterator i = tuDecl->decls_begin();
        i != tuDecl->decls_end(); ++i)
    {
        clang::NamedDecl* named_decl = llvm::dyn_cast<clang::NamedDecl>(*i);
        if (named_decl == 0) continue;
        // Filter out unsupported decls at the global namespace level
        clang::Decl::Kind kind = named_decl->getKind();
        switch (kind)
        {
            case (clang::Decl::Namespace):
            case (clang::Decl::CXXRecord):
            case (clang::Decl::Enum):
            case (clang::Decl::ClassTemplate):
            case (clang::Decl::Field):
            case (clang::Decl::Var):
            case (clang::Decl::VarTemplateSpecialization):
                HandleDecl(named_decl, PAR_NoReflect, nullptr);
                break;
            default:
                break;
        }
    }
    for (clang::DeclContext::decl_iterator i = tuDecl->decls_begin();
        i != tuDecl->decls_end(); ++i)
    {
        clang::NamedDecl* named_decl = llvm::dyn_cast<clang::NamedDecl>(*i);
        if (named_decl == 0) continue;
        // Filter out unsupported decls at the global namespace level
        clang::Decl::Kind kind = named_decl->getKind();
        switch (kind)
        {
            case (clang::Decl::Function):
            case (clang::Decl::ParmVar):
                HandleDecl(named_decl, PAR_NoReflect, nullptr);
                break;
            default:
                break;
        }
    }

    // 2. analyze
    for (auto& iter : datamap.files)
    {
        auto& source = iter.second;
        source->analyze(); 
    }

    for (auto& pair : datamap.files)
    {
        using namespace llvm;
        SmallString<1024> MetaPath(OutputPath + pair.first().str());
        sys::path::replace_extension(MetaPath, ".hlsl");
        SmallString<1024> MetaDir = MetaPath;
        sys::path::remove_filename(MetaDir);
        llvm::sys::fs::create_directories(MetaDir);
        std::ofstream of(MetaPath.str().str());
        of << ssl::compile(*pair.second);
    }
}

void Remove(std::string& str, const std::string& remove_str)
{
    for (size_t i; (i = str.find(remove_str)) != std::string::npos;)
        str.replace(i, remove_str.length(), "");
}

void Join(std::string& a, const std::string& b)
{
    if (!a.empty())
        a += ",";
    a += b;
}

bool isBuiltin(clang::NamedDecl* decl)
{
    for (auto attr : decl->attrs())
    {
        for (auto annotate : decl->specific_attrs<clang::AnnotateAttr>())
        {
            if (annotate->getAnnotation() == "sakura-shader")
            {
                auto iter = annotate->args_begin();
                if (iter != annotate->args_end())
                {
                    if (auto Literial = llvm::dyn_cast<clang::StringLiteral>((*iter)->IgnoreParenCasts()))
                    {
                        return Literial->getString() == "builtin";
                    }
                }
            }
        }
    }
    return false;
}

void ssl::ASTConsumer::HandleDecl(clang::NamedDecl* decl, ParseBehavior behavior, const clang::ASTRecordLayout* layout)
{
    if (decl->isInvalidDecl()) return;
    clang::NamedDecl* attrDecl = decl;
    std::string filename;
    // Resolve filename & abspath
    auto& SM = _ASTContext->getSourceManager();
    auto location = SM.getPresumedLoc(decl->getLocation());
    if (!location.isInvalid())
    {
        SmallString<1024> AbsolutePath(tooling::getAbsolutePath(location.getFilename()));
        llvm::sys::path::remove_dots(AbsolutePath, true);
        filename = llvm::sys::path::convert_to_slash(AbsolutePath.str());
        if (filename.empty()) return;
    }
    else
    {
        return;
    }

    // Locate DB
    if (datamap.files.find(filename) == datamap.files.end())
    {
        datamap.files[filename] = std::make_unique<SourceFile>(
            location.getFilename(), filename);
    }
    auto& db = datamap.files[filename];
    if (auto templateDecl = llvm::dyn_cast<clang::ClassTemplateDecl>(decl))
    {
        if (auto inner = templateDecl->getTemplatedDecl())
        {
            attrDecl = inner;
        }
    }

    // Create record
    ssl::Declare* declare = nullptr;
    switch (attrDecl->getKind())
    {
    case (clang::Decl::Namespace):
    {
    clang::DeclContext *declContext = decl->castToDeclContext(decl);
    for (clang::DeclContext::decl_iterator i = declContext->decls_begin();
        i != declContext->decls_end(); ++i) 
    {
        clang::NamedDecl* _named_decl = llvm::dyn_cast<clang::NamedDecl>(*i);
        HandleDecl(_named_decl, PAR_NoReflect, nullptr);
    }
    }
    break;
    case clang::Decl::Function:
    {
        declare = db->functions.emplace_back(
            new FunctionDeclare(attrDecl, db->abs_filename, &datamap)
        );
        break;
    }
    case clang::Decl::CXXRecord:
    {
        if (isBuiltin(attrDecl))
        {
            declare = db->types.emplace_back(
                new BuiltinDeclare(attrDecl, db->abs_filename, &datamap)
            );
        }
        else
        {
            declare = db->types.emplace_back(
                new StructureDeclare(attrDecl, db->abs_filename, &datamap)
            );
        }
        break;
    }
    case clang::Decl::Var:
    case clang::Decl::VarTemplateSpecialization:
    {
        if (auto tmpvar = llvm::dyn_cast<clang::VarDecl>(attrDecl))
        {
            auto parent = tmpvar->getParentFunctionOrMethod();
            if (parent == nullptr)
            {
                declare = db->vars.emplace_back(
                    new GlobalVarDeclare(attrDecl, db->abs_filename, &datamap)
                );
            }
        }
        break;
    }
    default: 
        break;
    }
}
