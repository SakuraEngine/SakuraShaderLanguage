#include "./../Source.h"
#include "./../Attributes/ShaderAttr.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include <iostream>

namespace ssl
{
VarDeclare::~VarDeclare()
{

}

VarDeclare::VarDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : Declare(decl, file_id, root)
{
    
}

GlobalVarDeclare::GlobalVarDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : VarDeclare(decl, file_id, root)
{

}

LocalVarDeclare::LocalVarDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : VarDeclare(decl, file_id, root)
{
    
}
}