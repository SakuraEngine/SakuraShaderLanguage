#include "SVAttr.h"

#include <iostream>

namespace ssl
{
SVAttribute::SVAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare)
    : ShaderAttribute(_this), declare(declare)
{

}

static ShaderAttribute::Store<SVAttribute> SVStore("sv", 
    [](clang::AnnotateAttr* attr, ssl::Declare* decl) -> ShaderAttribute*
    { 
        if (decl->getKind() != clang::Decl::ParmVar &&
            decl->getKind() != clang::Decl::Field &&
            decl->getKind() != clang::Decl::Var && /*output vars*/
            decl->getKind() != clang::Decl::VarTemplateSpecialization
        )
        {
            return nullptr;
        }
        return new SVAttribute(attr, decl); 
    }
);

}