#pragma once
#include "clang/Sema/ParsedAttr.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"

/*
[stage_in(N)]
{
    [attribute(0)];
    ...
}

[table(N)]
{
    tex<>;
    sampler;
    buffer<>;
    ...
}

[stage_out]
{
    [attribute(0)];
    [color(0)];
    ...
}
*/

namespace ssl
{
struct TableAttrInfo : public clang::ParsedAttrInfo
{
    TableAttrInfo();

    bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override;
    AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override;
};

struct StageInputAttrInfo : public clang::ParsedAttrInfo
{
    StageInputAttrInfo();

    bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override;
    AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override;
};

struct StageOutputAttrInfo : public clang::ParsedAttrInfo
{
    StageOutputAttrInfo();

    bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override;
    AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override;
};

struct BindingAttrInfo : public clang::ParsedAttrInfo
{
    virtual bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override;
    virtual AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override;
};

struct AttributeAttrInfo : public BindingAttrInfo
{
    AttributeAttrInfo();

    bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override;
    AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override;
};

/*
struct RegBindingAttrInfo : public BindingAttrInfo
{
    RegBindingAttrInfo();

    bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override;
    AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override;
};
*/
} // namespace ssl