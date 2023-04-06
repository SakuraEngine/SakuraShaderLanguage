#include "BuiltinAttributes.h"

namespace ssl
{
using namespace clang;

// Builtin
BuildinAttrInfo::BuildinAttrInfo()
    : clang::ParsedAttrInfo()
{
    NumArgs = 1;
    OptArgs = 0;
    HasCustomParsing = 0;
    IsTargetSpecific = 0;
    IsType = 0;
    IsStmt = 0;
    IsKnownToGCC = 1;
    static constexpr Spelling S[] = {
        { clang::ParsedAttr::AS_GNU, "builtin" },
        { clang::ParsedAttr::AS_C2x, "builtin" },
        { clang::ParsedAttr::AS_CXX11, "builtin" }
    };
    Spellings = S;
}

bool BuildinAttrInfo::diagAppertainsToDecl(Sema& S, const ParsedAttr& Attr, const Decl* D) const
{
    // This attribute appertains to functions only.
    if (!isa<FunctionDecl>(D))
    {
        S.Diag(Attr.getLoc(), diag::warn_attribute_wrong_decl_type_str)
        << Attr << "functions";
        return false;
    }
    auto N = Attr.getNormalizedFullName();
    (void)N;
    if (Attr.getNumArgs() > 0)
    {
        auto* BindingExpr = Attr.getArgAsExpr(0);
        if (auto BindingLiteral = dyn_cast<StringLiteral>(BindingExpr->IgnoreParenCasts()))
        {
            // ...
        }
        else
        {
            unsigned ID = S.getDiagnostics().getCustomDiagID(
            DiagnosticsEngine::Error, "first argument to the 'builtin' attribute must be a string literal");
            S.Diag(Attr.getLoc(), ID);
            return AttributeNotApplied;
        }
    }
    else
    {
        unsigned ID = S.getDiagnostics().getCustomDiagID(
        DiagnosticsEngine::Error, "must have a name argument");
        S.Diag(Attr.getLoc(), ID);
        return AttributeNotApplied;
    }
    // Handle the attribute
    return AttributeApplied;
    return true;
}

BuildinAttrInfo::AttrHandling BuildinAttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    auto Decl = dyn_cast<TranslationUnitDecl>(D->getDeclContext());
    // if (!Decl->isExternCContext())
    if (false)
    {
        unsigned ID = S.getDiagnostics().getCustomDiagID(
        DiagnosticsEngine::Error,
        "'builtin' attributes only allowed on function or methods at extern scope");
        S.Diag(Attr.getLoc(), ID);
        return AttributeNotApplied;
    }
    // Handle the attribute
    return AttributeApplied;
}

void BuildinAttrInfo::getPragmaAttributeMatchRules(llvm::SmallVectorImpl<std::pair<attr::SubjectMatchRule, bool>>& MatchRules, const LangOptions& LangOpts) const
{
    MatchRules.push_back(std::make_pair(attr::SubjectMatchRule_function, /*IsSupported=*/true));
    MatchRules.push_back(std::make_pair(attr::SubjectMatchRule_variable_is_parameter, /*IsSupported=*/true));
}

static ParsedAttrInfoRegistry::Add<ssl::BuildinAttrInfo> __builtinAttr("builtin", "Builtin intrin or function");

} // namespace ssl
