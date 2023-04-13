#include "./../Source.h"
#include "./../Attributes/ShaderAttr.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include <iostream>

namespace ssl
{
ParameterDeclare::ParameterDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : VarDeclare(decl, file_id, root)
{
    
}

const TypeDeclare* ParameterDeclare::getTypeDeclare() const
{
    if (auto param = llvm::dyn_cast<clang::ParmVarDecl>(decl))
    {
        auto type = param->getType()->getAs<clang::RecordType>();
        // decay
        if (!type) 
            type = param->getType()->getPointeeType()->getAs<clang::RecordType>();
        if (!type) 
            type = param->getType().getNonReferenceType()->getAs<clang::RecordType>();
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

}