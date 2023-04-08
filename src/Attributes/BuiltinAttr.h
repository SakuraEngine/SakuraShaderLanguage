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

    std::string getBuiltinName() const { return ShaderAttribute::GetStringArgFromAnnotate(_this, 0); }

    ssl::Declare* declare;
};

}