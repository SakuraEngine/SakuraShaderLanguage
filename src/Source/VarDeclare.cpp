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
    : TypedDeclare(decl, file_id, root)
{
    
}

TypeDeclare* VarDeclare::getTypeDeclare() const
{
    if (auto var = llvm::dyn_cast<clang::VarDecl>(decl))
    {
        auto type = var->getType()->getAs<clang::RecordType>();
        // decay
        if (!type) 
            type = var->getType()->getPointeeType()->getAs<clang::RecordType>();
        if (!type) 
            type = var->getType().getNonReferenceType()->getAs<clang::RecordType>();
        if (type)
        {
            if (auto record = type->getDecl())
            {
                return root->find(record);
            }
        }
        return nullptr;
    }
    return nullptr;
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