#pragma once
#include "clang/Sema/ParsedAttr.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"

namespace ssl
{
struct BuildinAttrInfo : public clang::ParsedAttrInfo
{
    BuildinAttrInfo();

    virtual bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override;
    virtual AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override;

    void getPragmaAttributeMatchRules(llvm::SmallVectorImpl<std::pair<clang::attr::SubjectMatchRule, bool>> &MatchRules, const clang::LangOptions &LangOpts) const override;
};

} // namespace ssl