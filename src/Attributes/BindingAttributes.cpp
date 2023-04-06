#include "BindingAttributes.h"

namespace ssl
{
using namespace clang;

// Table

TableAttrInfo::TableAttrInfo()
{
    OptArgs = 5;
    static constexpr Spelling S[] = {
        { clang::ParsedAttr::AS_GNU, "table" },
        { clang::ParsedAttr::AS_C2x, "table" },
        { clang::ParsedAttr::AS_CXX11, "table" },
        { clang::ParsedAttr::AS_CXX11, "table" }
    };
    Spellings = S;
}

bool TableAttrInfo::diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const
{
    // This attribute appertains to functions only.
    if (!isa<ParmVarDecl>(D)) 
    {
        S.Diag(Attr.getLoc(), diag::warn_attribute_wrong_decl_type_str)
            << Attr << "function input";
        return false;
    }
    return true;
}

TableAttrInfo::AttrHandling TableAttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    if (Attr.getNumArgs() > 0)
    {
        auto* BindingExpr = Attr.getArgAsExpr(0);
        if (auto BindingLiteral = dyn_cast<IntegerLiteral>(BindingExpr->IgnoreParenCasts()))
        {
            // ...
        }
        else
        {
            unsigned ID = S.getDiagnostics().getCustomDiagID(
                DiagnosticsEngine::Error, "first argument to the 'stage_in' attribute must be a string literal");
            S.Diag(Attr.getLoc(), ID);
            return AttributeNotApplied;
        }
    }
    // Handle the attribute
    return AttributeApplied;
}
static ParsedAttrInfoRegistry::Add<ssl::TableAttrInfo> __tableAttr("table", "Parameter Table");

// Stage Input

StageInputAttrInfo::StageInputAttrInfo()
{
    OptArgs = 5;
    static constexpr Spelling S[] = {
        { clang::ParsedAttr::AS_GNU, "stage_in" },
        { clang::ParsedAttr::AS_C2x, "stage_in" },
        { clang::ParsedAttr::AS_CXX11, "stage_in" },
        { clang::ParsedAttr::AS_CXX11, "stage_in" }
    };
    Spellings = S;
}

bool StageInputAttrInfo::diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const
{
    // This attribute appertains to functions only.
    if (!isa<ParmVarDecl>(D)) 
    {
        S.Diag(Attr.getLoc(), diag::warn_attribute_wrong_decl_type_str)
            << Attr << "function input";
        return false;
    }
    return true;
}

StageInputAttrInfo::AttrHandling StageInputAttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    if (Attr.getNumArgs() > 0)
    {
        auto* BindingExpr = Attr.getArgAsExpr(0);
        if (auto BindingLiteral = dyn_cast<IntegerLiteral>(BindingExpr->IgnoreParenCasts()))
        {
            // ...
        }
        else
        {
            unsigned ID = S.getDiagnostics().getCustomDiagID(
                DiagnosticsEngine::Error, "first argument to the 'stage_in' attribute must be a string literal");
            S.Diag(Attr.getLoc(), ID);
            return AttributeNotApplied;
        }
    }
    // Handle the attribute
    return AttributeApplied;
}
static ParsedAttrInfoRegistry::Add<ssl::StageInputAttrInfo> __stageInputAttr("stage_in", "Stage Input");

// Stage Output

StageOutputAttrInfo::StageOutputAttrInfo()
{
    OptArgs = 5;
    static constexpr Spelling S[] = {
        { clang::ParsedAttr::AS_GNU, "stage_out" },
        { clang::ParsedAttr::AS_C2x, "stage_out" },
        { clang::ParsedAttr::AS_CXX11, "stage_out" },
        { clang::ParsedAttr::AS_CXX11, "stage_out" }
    };
    Spellings = S;
}

bool StageOutputAttrInfo::diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const
{
    // This attribute appertains to functions only.
    if (!isa<ParmVarDecl>(D)) 
    {
        S.Diag(Attr.getLoc(), diag::warn_attribute_wrong_decl_type_str)
            << Attr << "function input";
        return false;
    }
    return true;
}

StageOutputAttrInfo::AttrHandling StageOutputAttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    if (Attr.getNumArgs() > 0)
    {
        unsigned ID = S.getDiagnostics().getCustomDiagID(
            DiagnosticsEngine::Error, "multi return values are not supported yet");
        S.Diag(Attr.getLoc(), ID);
        return AttributeNotApplied;
    }
    // Handle the attribute
    return AttributeApplied;
}
static ParsedAttrInfoRegistry::Add<ssl::StageInputAttrInfo> __stageOutputAttr("stage_out", "Stage Output");

// Binding

bool BindingAttrInfo::diagAppertainsToDecl(Sema& S, const ParsedAttr& Attr, const Decl* D) const
{
    // This attribute appertains to functions only.
    if (!isa<FieldDecl>(D))
    {
        S.Diag(Attr.getLoc(), diag::warn_attribute_wrong_decl_type_str)
        << Attr << "struct fields";
        return false;
    }
    return true;
}

BindingAttrInfo::AttrHandling BindingAttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    if (!D->getDeclContext()->isRecord())
    {
        unsigned ID = S.getDiagnostics().getCustomDiagID(
            DiagnosticsEngine::Error,
            "'binding' attributes only allowed inside records");
        S.Diag(Attr.getLoc(), ID);
        return AttributeNotApplied;
    }
    // Handle the attribute
    return AttributeApplied;
}

// (Vertex) Attribute

AttributeAttrInfo::AttributeAttrInfo()
{
    OptArgs = 5;
    static constexpr Spelling S[] = {
        { clang::ParsedAttr::AS_GNU, "attribute" },
        { clang::ParsedAttr::AS_C2x, "attribute" },
        { clang::ParsedAttr::AS_CXX11, "attribute" },
        { clang::ParsedAttr::AS_CXX11, "attribute" }
    };
    Spellings = S;
}

bool AttributeAttrInfo::diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const
{
    const bool super = BindingAttrInfo::diagAppertainsToDecl(S, Attr, D);
    return super;
}

AttributeAttrInfo::AttrHandling AttributeAttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    auto super = BindingAttrInfo::handleDeclAttribute(S, D, Attr);
    if (super == AttributeNotApplied) return AttributeNotApplied;

    if (Attr.getNumArgs() > 0)
    {
        auto* BindingExpr = Attr.getArgAsExpr(0);
        if (auto BindingLiteral = dyn_cast<IntegerLiteral>(BindingExpr->IgnoreParenCasts()))
        {
            // ...
        }
        else
        {
            unsigned ID = S.getDiagnostics().getCustomDiagID(
                DiagnosticsEngine::Error, "first argument to the 'attribute' attribute must be a string literal");
            S.Diag(Attr.getLoc(), ID);
            return AttributeNotApplied;
        }
    }
    // Handle the attribute
    return AttributeApplied;
}
static ParsedAttrInfoRegistry::Add<ssl::AttributeAttrInfo> __attrAttr("attribute", "Vertex Attribute");


} // namespace ssl