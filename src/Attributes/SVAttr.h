#pragma once
#include "ShaderAttr.h"

namespace ssl
{
// 1.
// frag_main([[sv(position)]] float4 pos)
// 2.
// struct VertexOutput { [[sv(position)]] float4 pos; }; 
// frag_main([[stage_in]] parameters)
struct SVAttribute : public ShaderAttribute
{
    SVAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare);
    
    ShaderAttributeKind getKind() const override { return kSVShaderAttribute; }
    static bool classof(const ShaderAttribute* A)
    {
        return A->getKind() == ssl::kSVShaderAttribute;
    }

    ssl::Declare* declare;
};

}