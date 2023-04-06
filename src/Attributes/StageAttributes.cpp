#include "StageAttributes.h"

namespace ssl
{
using namespace clang;

bool StageAttrInfo::diagAppertainsToDecl(Sema& S, const ParsedAttr& Attr, const Decl* D) const
{
    // This attribute appertains to functions only.
    if (!isa<FunctionDecl>(D))
    {
        S.Diag(Attr.getLoc(), diag::warn_attribute_wrong_decl_type_str) << Attr << "functions";
        return false;
    }
    return true;
}

FragmentStageAttrInfo::AttrHandling StageAttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    if (!D->getDeclContext()->isFileContext())
    {
        unsigned ID = S.getDiagnostics().getCustomDiagID(
            DiagnosticsEngine::Error,
            "'shader stage' attributes only allowed at file scope");
        S.Diag(Attr.getLoc(), ID);
        return AttributeNotApplied;
    }
    // Handle the attribute
    return AttributeApplied;
}

// Vertex Stage

VertexStageAttrInfo::VertexStageAttrInfo()
{
    OptArgs = 5;
    static constexpr Spelling S[] = {
        { clang::ParsedAttr::AS_GNU, "stage::vertex" },
        { clang::ParsedAttr::AS_C2x, "stage::vertex" },
        { clang::ParsedAttr::AS_CXX11, "stage::vertex" },
        { clang::ParsedAttr::AS_CXX11, "stage::vertex" }
    };
    Spellings = S;
}

bool VertexStageAttrInfo::diagAppertainsToDecl(Sema& S, const ParsedAttr& Attr, const Decl* D) const
{
    const bool super = StageAttrInfo::diagAppertainsToDecl(S, Attr, D);
    return super;
}

VertexStageAttrInfo::AttrHandling VertexStageAttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    auto super = StageAttrInfo::handleDeclAttribute(S, D, Attr);
    if (super == AttributeNotApplied) return AttributeNotApplied;

    // Handle the attribute
    return AttributeApplied;
}

static ParsedAttrInfoRegistry::Add<ssl::VertexStageAttrInfo> __vertAttr("stage::vertex", "mark a function as fragment stage");

// Fragment Stage

FragmentStageAttrInfo::FragmentStageAttrInfo()
{
    OptArgs = 5;
    static constexpr Spelling S[] = {
        { clang::ParsedAttr::AS_GNU, "stage::fragment" },
        { clang::ParsedAttr::AS_C2x, "stage::fragment" },
        { clang::ParsedAttr::AS_CXX11, "stage::fragment" },
        { clang::ParsedAttr::AS_CXX11, "stage::fragment" }
    };
    Spellings = S;
}

bool FragmentStageAttrInfo::diagAppertainsToDecl(Sema& S, const ParsedAttr& Attr, const Decl* D) const
{
    const bool super = StageAttrInfo::diagAppertainsToDecl(S, Attr, D);
    return super;
}

FragmentStageAttrInfo::AttrHandling FragmentStageAttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    auto super = StageAttrInfo::handleDeclAttribute(S, D, Attr);
    if (super == AttributeNotApplied) return AttributeNotApplied;

    // Handle the attribute
    return AttributeApplied;
}

static ParsedAttrInfoRegistry::Add<ssl::FragmentStageAttrInfo> __fragAttr("stage::fragment", "mark a function as fragment stage");

}
