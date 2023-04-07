#include "BuiltinAttr.h"
#include "clang/AST/Type.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Tooling/Tooling.h"

namespace ssl
{
BuiltinAttribute::BuiltinAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare)
    : ShaderAttribute(_this), declare(declare)
{

}

static ShaderAttribute::Store<BuiltinAttribute> BuiltinStore("builtin", 
    [](clang::AnnotateAttr* attr, ssl::Declare* decl) -> ShaderAttribute*
    { 
        if (decl->getKind() != clang::Decl::Function &&
            decl->getKind() != clang::Decl::Record &&
            decl->getKind() != clang::Decl::CXXRecord
        )
        {
            return nullptr;
        }
        return new BuiltinAttribute(attr, decl); 
    }
);

}