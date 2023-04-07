#pragma once
#include "ShaderAttr.h"

namespace ssl
{

struct BuiltinAttribute : public ShaderAttribute
{
    BuiltinAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare);
    
    ShaderAttributeKind getKind() const override { return kBuiltinShaderAttribute; }
    static bool classof(const ShaderAttribute* A)
    {
        return A->getKind() == ssl::kBuiltinShaderAttribute;
    }
    
    ssl::Declare* declare;
};

}