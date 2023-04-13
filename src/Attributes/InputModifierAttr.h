#pragma once
#include "ShaderAttr.h"

namespace ssl
{

struct InputModifierAttribute : public ShaderAttribute
{
    InputModifierAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare);
    
    ShaderAttributeKind getKind() const override { return kInputModifierAttribute; }
    static bool classof(const ShaderAttribute* A)
    {
        return A->getKind() == ssl::kInputModifierAttribute;
    }

    AccessType getAcessType() const 
    { 
        return access_type;
    }

protected:
    ssl::Declare* declare;
    AccessType access_type;
};

}