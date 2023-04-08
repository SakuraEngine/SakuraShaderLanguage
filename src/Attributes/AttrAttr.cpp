#include "AttrAttr.h"
#include "./../Source.h"

namespace ssl
{
AttributeAttribute::AttributeAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare)
    : ShaderAttribute(_this), declare(declare)
{

}

static ShaderAttribute::Store<AttributeAttribute> AttrAttrStore("attribute", 
    [](clang::AnnotateAttr* attr, ssl::Declare* decl) -> ShaderAttribute*
    { 
        if (decl->getKind() != clang::Decl::Field)
        {
            return nullptr;
        }
        return new AttributeAttribute(attr, decl); 
    }
);

}