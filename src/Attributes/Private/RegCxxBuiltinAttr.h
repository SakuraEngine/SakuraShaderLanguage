#pragma once
#include "./../ShaderAttr.h"

namespace ssl
{
// [[_reg_cxx_builtin]]
struct RegCxxBuiltinAttribute : public ShaderAttribute
{
    RegCxxBuiltinAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare);
    
    ShaderAttributeKind getKind() const override { return kRegCxxBuiltinAttribute; }
    static bool classof(const ShaderAttribute* A)
    {
        return A->getKind() == ssl::kRegCxxBuiltinAttribute;
    }

    ssl::Declare* declare;
};

}