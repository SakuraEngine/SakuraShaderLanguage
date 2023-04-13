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

const TypeDeclare* VarDeclare::getTypeDeclare() const
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

void VarDeclare::queryStageInOut(bool* in, bool* out) const
{
    auto QualType = llvm::dyn_cast<clang::ParmVarDecl>(this->getDecl())->getType();
    auto input_attrs = this->findAttributes(kStageInputAttribute);
    auto output_attrs = this->findAttributes(kStageOutputAttribute);
    bool isOutput = QualType->isReferenceType(); // func(T& t), t must be an output
    isOutput |= (bool)output_attrs.size(); // [[stage_out(i)]] T global; t must an output
    isOutput &= !(bool)input_attrs.size(); // [[stage_in(i)]] T global; t must be an input
    if (isOutput)
    {
        if (auto asRef = QualType->getAs<clang::ReferenceType>())
        {
            if (out) *out = true;
        }
    }
    else
    {
        if (in) *in = true;
    }
}

bool VarDeclare::isStageInput() const
{
    bool in = false, out = false;
    queryStageInOut(&in, &out);
    return in;
}

bool VarDeclare::isStageOutput() const
{
    bool in = false, out = false;
    queryStageInOut(&in, &out);
    return out;
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