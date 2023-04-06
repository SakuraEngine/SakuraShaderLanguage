#include "ASTConsumer.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/Type.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Path.h"
#include <vector>
#include <iostream>

void ssl::ASTConsumer::HandleTranslationUnit(ASTContext& ctx)
{
    _ASTContext = &ctx;
    auto tuDecl = ctx.getTranslationUnitDecl();
    for (clang::DeclContext::decl_iterator i = tuDecl->decls_begin();
         i != tuDecl->decls_end(); ++i)
    {
        clang::NamedDecl* named_decl = llvm::dyn_cast<clang::NamedDecl>(*i);
        if (named_decl == 0)
            continue;

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
    clang::Decl::Kind kind = decl->getKind();
    std::string attr;
    clang::NamedDecl* attrDecl = decl;
    std::string fileName;
    std::string absPath;
    // Resolve filename & abspath
    {
        auto location = _ASTContext->getSourceManager().getPresumedLoc(decl->getLocation());
        if (!location.isInvalid())
        {
            SmallString<1024> AbsolutePath(tooling::getAbsolutePath(location.getFilename()));
            llvm::sys::path::remove_dots(AbsolutePath, true);
            fileName = absPath = llvm::sys::path::convert_to_slash(AbsolutePath.str());
            if (fileName.empty()) return;
        }
        else
        {
            return;
        }
    }
    //
    auto& db = datamap[fileName];
    if (auto templateDecl = llvm::dyn_cast<clang::ClassTemplateDecl>(decl))
    {
        if (auto inner = templateDecl->getTemplatedDecl())
            attrDecl = inner;
    }
    ParseBehavior childBehavior = behavior == PAR_Normal ? PAR_NoReflect : behavior;
    for (auto attr : attrDecl->attrs())
    {
        std::cout << attr << std::endl;
    }
}

void ssl::ASTConsumer::HandleRecord(clang::NamedDecl* decl, std::vector<std::string>& attrStack, ParseBehavior behavior, const clang::ASTRecordLayout* layout)
{
}