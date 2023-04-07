#include "defines.h"
#include "Attributes/ShaderAttr.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Stmt.h"

#include <iostream>

namespace ssl
{
SourceFile::~SourceFile()
{
    for (auto function : functions) delete function;
    for (auto structure : structs) delete structure;
}

struct FunctionDeclare* SourceFile::find(clang::FunctionDecl* decl) const
{
    for (auto function : functions)
    {
        if (function->getDecl() == decl)
        {
            return function;
        }
    }
    return nullptr;
}

Declare::Declare(clang::NamedDecl* decl, std::string_view file_id)
    : decl(decl), file_id(file_id)
{
    kind = decl->getKind();
    for (auto attr : decl->attrs())
    {
        for (auto annotate : decl->specific_attrs<clang::AnnotateAttr>())
        {
            attributes.emplace_back(ShaderAttribute::Create(annotate, this));
        }
    }
}

StructDeclare::StructDeclare(clang::NamedDecl* decl, std::string_view file_id)
    : Declare(decl, file_id)
{
    if (auto record = llvm::dyn_cast<clang::RecordDecl>(decl))
    {
        for (auto field : record->fields())
        {
            fields.emplace_back(new FieldDeclare(field, file_id));
        }
    }
}

StructDeclare::~StructDeclare()
{
    for (auto field : fields) delete field;
}

VarTemplateDeclare::~VarTemplateDeclare()
{
    delete var;
}

VarTemplateDeclare::VarTemplateDeclare(clang::NamedDecl* decl, std::string_view file_id)
    : Declare(decl, file_id)
{
    if (auto tempVar = llvm::dyn_cast<clang::VarTemplateDecl>(decl))
    {
        auto inner = tempVar->getTemplatedDecl();
        var = new VarDeclare(inner, file_id);
    }
}

FunctionDeclare::FunctionDeclare(clang::NamedDecl* decl, std::string_view file_id)
    : Declare(decl, file_id)
{
    if (auto function = llvm::dyn_cast<clang::FunctionDecl>(decl))
    {
        for (auto parameter : function->parameters())
        {
            parameters.emplace_back(new ParameterDeclare(parameter, file_id));
        }
    }
}

FunctionDeclare::~FunctionDeclare()
{
    for (auto parameter : parameters) delete parameter;
    for (auto var : vars) delete var;
}

}