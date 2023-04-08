#include "StageAttr.h"
#include "./../Source.h"

namespace ssl
{
StageAttribute::StageAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare)
    : ShaderAttribute(_this), declare(declare)
{

}

static ShaderAttribute::Store<StageAttribute> StageAttrStore("stage", 
    [](clang::AnnotateAttr* attr, ssl::Declare* decl) -> ShaderAttribute*
    { 
        if (decl->getKind() != clang::Decl::Function)
        {
            return nullptr;
        }
        return new StageAttribute(attr, decl); 
    }
);

StageInputAttribute::StageInputAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare)
    : ShaderAttribute(_this), declare(declare)
{
    
}

static ShaderAttribute::Store<StageInputAttribute> StageInputAttrStore("stage_input", 
    [](clang::AnnotateAttr* attr, ssl::Declare* decl) -> ShaderAttribute*
    { 
        if (decl->getKind() != clang::Decl::ParmVar)
        {
            return nullptr;
        }
        return new StageInputAttribute(attr, decl); 
    }
);

}