#include "ASTConsumer.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/DeclTemplate.h"
#include "Attributes/ShaderAttr.h"

void ssl::ASTConsumer::HandleTranslationUnit(ASTContext& ctx)
{
    _ASTContext = &ctx;
    auto tuDecl = ctx.getTranslationUnitDecl();
    for (clang::DeclContext::decl_iterator i = tuDecl->decls_begin();
        i != tuDecl->decls_end(); ++i)
    {
        clang::NamedDecl* named_decl = llvm::dyn_cast<clang::NamedDecl>(*i);
        if (named_decl == 0) continue;

        // Filter out unsupported decls at the global namespace level
        clang::Decl::Kind kind = named_decl->getKind();
        std::vector<std::string> newStack;
        switch (kind)
        {
            case (clang::Decl::Namespace):
            case (clang::Decl::CXXRecord):
            case (clang::Decl::Function):
            case (clang::Decl::Enum):
            case (clang::Decl::ClassTemplate):
            case (clang::Decl::ParmVar):
            case (clang::Decl::Field):
            case (clang::Decl::Var):
            case (clang::Decl::VarTemplateSpecialization):
                HandleRecord(named_decl, newStack, PAR_NoReflect, nullptr);
                break;
            default:
                break;
        }
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

std::string GetTypeName(clang::QualType type, clang::ASTContext* ctx)
{
    type = type.getCanonicalType();
    auto baseName = type.getAsString(ctx->getLangOpts());
    Remove(baseName, "struct ");
    Remove(baseName, "class ");
    return baseName;
}

std::string GetRawTypeName(clang::QualType type, clang::ASTContext* ctx)
{
    if (type->isPointerType() || type->isReferenceType())
        type = type->getPointeeType();
    type = type.getUnqualifiedType();
    auto baseName = type.getAsString(ctx->getLangOpts());
    Remove(baseName, "struct ");
    Remove(baseName, "class ");
    return baseName;
}

void ssl::ASTConsumer::HandleDecl(clang::NamedDecl* decl, std::vector<std::string>& attrStack, ParseBehavior behavior, const clang::ASTRecordLayout* layout)
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
    if (datamap.find(filename) == datamap.end())
    {
        datamap[filename] = SourceFile(location.getFilename(), filename);
    }
    auto& db = datamap[filename];
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
    case clang::Decl::Function:
    {
        declare = db.functions.emplace_back(new FunctionDeclare(attrDecl, filename));
        break;
    }
    case clang::Decl::CXXRecord:
    {
        declare = db.structs.emplace_back(new StructDeclare(attrDecl, filename));
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
                db.vars.emplace_back(new VarDeclare(attrDecl, filename));
            }
            else if (auto funcDecl = db.find(llvm::dyn_cast<clang::FunctionDecl>(parent)))
            {
                funcDecl->vars.emplace_back(new VarDeclare(attrDecl, filename));
            }
        }
        break;
    }
    default: 
        break;
    }
}

void ssl::ASTConsumer::HandleRecord(clang::NamedDecl* decl, std::vector<std::string>& attrStack, ParseBehavior behavior, const clang::ASTRecordLayout* layout)
{
    HandleDecl(decl, attrStack, behavior, layout);
}