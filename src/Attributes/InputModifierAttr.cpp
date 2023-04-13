#include "InputModifierAttr.h"
#include "clang/AST/Type.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Tooling/Tooling.h"
#include "./../Source.h"

namespace ssl
{
InputModifierAttribute::InputModifierAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare)
    : ShaderAttribute(_this), declare(declare)
{
    auto type = ShaderAttribute::GetStringArgFromAnnotate(_this, 0);
    if (type == "in")
    {
        access_type = kAccessReading;
    }
    else if (type == "out")
    {
        access_type = kAccessWriting;
    }
    else if (type == "inout")
    {
        access_type = kAccessReadWrite;
    }
    else
    {
        access_type = kAcessTypeInvalid;
    }
}

static ShaderAttribute::Store<InputModifierAttribute> InputModifierStore("input_modifier", 
    [](clang::AnnotateAttr* attr, ssl::Declare* decl) -> ShaderAttribute*
    { 
        if (decl->getKind() != clang::Decl::Var &&
            decl->getKind() != clang::Decl::VarTemplateSpecialization &&
            decl->getKind() != clang::Decl::ParmVar
        )
        {
            return nullptr;
        }
        return new InputModifierAttribute(attr, decl); 
    }
);

}