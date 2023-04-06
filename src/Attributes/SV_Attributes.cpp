#include "SV_Attributes.h"

namespace ssl
{
using namespace clang;


bool SV_AttrInfo::diagAppertainsToDecl(Sema& S, const ParsedAttr& Attr, const Decl* D) const
{
    // This attribute appertains to functions only.
    if (!isa<ParmVarDecl>(D) && !isa<FieldDecl>(D)) 
    {
        S.Diag(Attr.getLoc(), diag::warn_attribute_wrong_decl_type_str)
            << Attr << "function inputs or struct fields";
        return false;
    }
    return true;
}

SV_AttrInfo::AttrHandling SV_AttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    if (!D->getDeclContext()->isFileContext())
    {
        unsigned ID = S.getDiagnostics().getCustomDiagID(
            DiagnosticsEngine::Error,
            "'system value' attributes only allowed at file scope");
            S.Diag(Attr.getLoc(), ID);
        return AttributeNotApplied;
    }
    // Handle the attribute
    return AttributeApplied;
}

// SV_Position

SVPosition_AttrInfo::SVPosition_AttrInfo()
{
    OptArgs = 5;
    static constexpr Spelling S[] = {
        { clang::ParsedAttr::AS_GNU, "sv::position" },
        { clang::ParsedAttr::AS_C2x, "sv::position" },
        { clang::ParsedAttr::AS_CXX11, "sv::position" },
        { clang::ParsedAttr::AS_CXX11, "sv::position" }
    };
    Spellings = S;
}

bool SVPosition_AttrInfo::diagAppertainsToDecl(Sema& S, const ParsedAttr& Attr, const Decl* D) const
{
    const bool super = SV_AttrInfo::diagAppertainsToDecl(S, Attr, D);
    return super;
}

SVPosition_AttrInfo::AttrHandling SVPosition_AttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    auto super = SV_AttrInfo::handleDeclAttribute(S, D, Attr);
    if (super == AttributeNotApplied) return AttributeNotApplied;

    // Handle the attribute
    return AttributeApplied;
}

static ParsedAttrInfoRegistry::Add<ssl::SVPosition_AttrInfo> __svPosAttr("sv::position", "SV_Position");

// SV_VertexID

SVVertexID_AttrInfo::SVVertexID_AttrInfo()
{
    OptArgs = 5;
    static constexpr Spelling S[] = {
        { clang::ParsedAttr::AS_GNU, "sv::vertex_id" },
        { clang::ParsedAttr::AS_C2x, "sv::vertex_id" },
        { clang::ParsedAttr::AS_CXX11, "sv::vertex_id" },
        { clang::ParsedAttr::AS_CXX11, "sv::vertex_id" }
    };
    Spellings = S;
}

bool SVVertexID_AttrInfo::diagAppertainsToDecl(Sema& S, const ParsedAttr& Attr, const Decl* D) const
{
    const bool super = SV_AttrInfo::diagAppertainsToDecl(S, Attr, D);
    return super;
}

SVPosition_AttrInfo::AttrHandling SVVertexID_AttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
{
    auto super = SV_AttrInfo::handleDeclAttribute(S, D, Attr);
    if (super == AttributeNotApplied) return AttributeNotApplied;

    // Handle the attribute
    return AttributeApplied;
}

static ParsedAttrInfoRegistry::Add<ssl::SVVertexID_AttrInfo> __svVertexIDAttr("sv::vertex_id", "SV_VertexID");


}
