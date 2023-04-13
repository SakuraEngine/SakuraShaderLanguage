#include "./../Source.h"
#include "./../Attributes/ShaderAttr.h"
#include "./../Attributes/InputModifierAttr.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include <iostream>

namespace ssl
{

class LocalVarRecorder : public clang::RecursiveASTVisitor<LocalVarRecorder>
{
public:
    LocalVarRecorder(FunctionDeclare* func) : func(func) {}
    bool VisitVarDecl(clang::VarDecl* decl)
    {
        if (decl->hasLocalStorage())
        {
            func->local_vars.emplace_back(
                new LocalVarDeclare(decl, func->file_id, func->root)
            );
        }
        return true;
    }    

    /*
    bool VisitDeclRefExpr(clang::DeclRefExpr* expr)
    {
        if (auto param_var = llvm::dyn_cast<clang::ParmVarDecl>(expr->getDecl()))
        {
            func->param_vars.emplace_back(
                new LocalVarDeclare(param_var, func->file_id, func->root)
            );
        }
        return true;
    }
    */
private:
    FunctionDeclare* func;
};

FunctionDeclare::FunctionDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : Declare(decl, file_id, root)
{
    if (auto function = llvm::dyn_cast<clang::FunctionDecl>(decl))
    {
        for (auto parameter : function->parameters())
        {
            parameters.emplace_back(new ParameterDeclare(parameter, file_id, root));
        }
        if (auto def = function->getDefinition())
        {
            LocalVarRecorder visitor(this);
            visitor.TraverseStmt(function->getBody());
        }
    }
}

FunctionDeclare::~FunctionDeclare()
{
    for (auto parameter : parameters) delete parameter;
    for (auto local_var : local_vars) delete local_var;
}


AccessType FunctionDeclare::getAccessType(ParameterDeclare* param) const
{
    AccessType t = kAccessNone;
    if (auto input_modifier = param->findAttribute<InputModifierAttribute>(kInputModifierAttribute))
    {
        t = input_modifier->getAcessType();
        if (t != kAccessNone && t != kAcessTypeInvalid)
        {
            return t;
        }
    }
    if (auto pdecl = llvm::dyn_cast<clang::ParmVarDecl>(param->getDecl()))
    {
        if (pdecl->getType()->isReferenceType()) return kAccessWriting;
        return kAccessReading;
    }
    return t;
}

AccessType FunctionDeclare::getAccessType(VarDeclare* var) const
{
    AccessType t = kAccessNone;
    if (auto input_modifier = var->findAttribute<InputModifierAttribute>(kInputModifierAttribute))
    {
        t = input_modifier->getAcessType();
        if (t != kAccessNone && t != kAcessTypeInvalid)
        {
            return t;
        }
    }        
    if (auto vdecl = llvm::dyn_cast<clang::VarDecl>(var->getDecl()))
    {
        if (vdecl->getType()->isReferenceType()) return kAccessWriting;
        return kAccessReading;
    }
    return t;
}

class VarRecorder : public clang::RecursiveASTVisitor<VarRecorder>
{
public:
    VarRecorder(FunctionDeclare* func) : func(func) {}
    bool VisitDeclRefExpr(clang::DeclRefExpr* expr)
    {
        auto value_decl = llvm::dyn_cast<clang::VarDecl>(expr->getDecl());
        if (auto var = func->root->find(value_decl))
        {
            func->outter_vars.emplace_back(var);
        }
        return true;
    }
private:
    FunctionDeclare* func;
};

void FunctionDeclare::analyze(SourceFile* src)
{
    if (auto function = llvm::dyn_cast<clang::FunctionDecl>(decl))
    {
        VarRecorder visitor(this);
        visitor.TraverseStmt(function->getBody());
    }
}

}