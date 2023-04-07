#include "StageAttr.h"

namespace ssl
{
StageAttribute::StageAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare)
    : ShaderAttribute(_this), declare(declare)
{

}

static ShaderAttribute::Store<StageAttribute> SVStore("stage", 
    [](clang::AnnotateAttr* attr, ssl::Declare* decl) -> ShaderAttribute*
    { 
        if (decl->getKind() != clang::Decl::Function)
        {
            return nullptr;
        }
        return new StageAttribute(attr, decl); 
    }
);

}