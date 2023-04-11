#include "./../Source.h"
#include "./../Attributes/ShaderAttr.h"
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
    // bool VisitDeclRefExpr(clang::DeclRefExpr* expr);
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

LocalVarDeclare* FunctionDeclare::findLocalVar(clang::NamedDecl* decl)
{
    for (auto var : local_vars)
    {
        if (var->getDecl() == decl)
        {
            return var;
        }
    }
    return nullptr;
}

class VarRWRecorder : public clang::RecursiveASTVisitor<VarRWRecorder>
{
public:
    VarRWRecorder(FunctionDeclare* func) : func(func) {}
    bool VisitDeclRefExpr(clang::DeclRefExpr* expr)
    {
        auto value_decl = llvm::dyn_cast<clang::VarDecl>(expr->getDecl());
        if (auto var = func->root->find(value_decl))
        {
            if (expr->isLValue())
            {
                func->write_vars.emplace_back(var);
            }
            else
            {
                func->read_vars.emplace_back(var);
            }
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
        VarRWRecorder visitor(this);
        visitor.TraverseStmt(function->getBody());
    }
}

}