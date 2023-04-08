#pragma once
#include "ShaderAttr.h"

namespace ssl
{
// [[attribute("color", 0)]]
struct AttributeAttribute : public ShaderAttribute
{
    AttributeAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare);
    
    ShaderAttributeKind getKind() const override { return kAttributeAttribute; }
    static bool classof(const ShaderAttribute* A)
    {
        return A->getKind() == ssl::kAttributeAttribute;
    }

    std::string getSemantic() const { return ShaderAttribute::GetStringArgFromAnnotate(_this, 0); }
    uint64_t getAttributeIndex() const { return ShaderAttribute::GetIntArgFromAnnotate(_this, 1); }

    ssl::Declare* declare;
};

}