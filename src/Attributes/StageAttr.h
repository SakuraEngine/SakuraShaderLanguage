#pragma once
#include "ShaderAttr.h"

namespace ssl
{

struct StageAttribute : public ShaderAttribute
{
    StageAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare);
    
    ShaderAttributeKind getKind() const override { return kStageShaderAttribute; }

    static bool classof(const ShaderAttribute* A)
    {
        return A->getKind() == ssl::kStageShaderAttribute;
    }

    ssl::Declare* declare;
};

}