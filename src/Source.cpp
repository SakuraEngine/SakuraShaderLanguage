#include "Source.h"
#include "Attributes/ShaderAttr.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Stmt.h"

#include <iostream>

namespace ssl
{
SourceFile::~SourceFile()
{
    for (auto function : functions)
        delete function;
    for (auto structure : structs) 
        delete structure;
}

void SourceFile::analyze()
{
    // 1. Iterate and analyze all stages
    for (auto function : functions)
    {
        auto stage_attrs = function->findAttributes(kStageShaderAttribute);
        for (auto attr : stage_attrs)
        {
            auto& newStage = analysis.stages.emplace_back();
            newStage.function = function;
            for (auto param : function->getParameters())
            {
                auto QualType = llvm::dyn_cast<clang::ParmVarDecl>(param->getDecl())->getType();
                auto input_attrs = param->findAttributes(kStageInputAttribute);
                auto output_attrs = param->findAttributes(kStageOutputAttribute);
                auto sv_attrs = param->findAttributes(kSVShaderAttribute);
                (void)input_attrs; // TODO: extract input attributes
                bool isOutput = QualType->isReferenceType(); // func(T& t), t must be an output
                isOutput |= (bool)output_attrs.size(); // [[stage_out(i)]] T global; t must an output
                isOutput &= !(bool)input_attrs.size(); // [[stage_in(i)]] T global; t must be an input
                for (auto sv_attr : sv_attrs)
                {
                    auto semantic = sv_attr->GetStringArgFromAnnotate(0);
                    if (semantic == "target")
                    {
                        isOutput = true;
                    }
                }
                if (isOutput)
                {
                    if (auto asRef = QualType->getAs<clang::ReferenceType>())
                        {
                        newStage.outputs.emplace_back(
                            AnalysisStageOutput{ param, nullptr, nullptr }
                        );
                    }
                }
                else
                {
                    newStage.inputs.emplace_back(
                        AnalysisStageInput{ param, nullptr }
                    );
                }
            }
        }
    }
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

struct StructDeclare* SourceFile::find(clang::RecordDecl* decl) const
{
    for (auto structure : structs)
    {
        if (structure->getDecl() == decl)
        {
            return structure;
        }
    }
    return nullptr;
}

Declare::~Declare()
{
    for (auto attr : attributes)
    {
        delete attr;
    }
}

Declare::Declare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : decl(decl), file_id(file_id), root(root)
{
    _debug_name = decl->getNameAsString();
    kind = decl->getKind();
    for (auto attr : decl->attrs())
    {
        for (auto annotate : decl->specific_attrs<clang::AnnotateAttr>())
        {
            if (annotate->getAnnotation() == "sakura-shader")
            {
                attributes.emplace_back(ShaderAttribute::Create(annotate, this));
            }
        }
    }
}

StructDeclare::StructDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : Declare(decl, file_id, root)
{
    if (auto record = llvm::dyn_cast<clang::RecordDecl>(decl))
    {
        for (auto field : record->fields())
        {
            fields.emplace_back(new FieldDeclare(field, file_id, root));
        }
    }
}

ShaderAttribute* Declare::findAttribute(ShaderAttributeKind kind)
{
    for (auto attr : attributes)
    {
        if (attr->getKind() == kind)
        {
            return attr;
        }
    }
    return nullptr;
}

llvm::SmallVector<ShaderAttribute*> Declare::findAttributes(ShaderAttributeKind kind)
{
    llvm::SmallVector<ShaderAttribute*> result;
    for (auto attr : attributes)
    {
        if (attr->getKind() == kind)
        {
            result.emplace_back(attr);
        }
    }
    return result;
}

StructDeclare::~StructDeclare()
{
    for (auto field : fields) delete field;
}

VarTemplateDeclare::~VarTemplateDeclare()
{
    delete var;
}

VarTemplateDeclare::VarTemplateDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : Declare(decl, file_id, root)
{
    if (auto tempVar = llvm::dyn_cast<clang::VarTemplateDecl>(decl))
    {
        auto inner = tempVar->getTemplatedDecl();
        var = new VarDeclare(inner, file_id, root);
    }
}

StructDeclare* FieldDeclare::getStructDeclare() const
{
    if (auto field = llvm::dyn_cast<clang::FieldDecl>(decl))
    {
        if (auto type = field->getType()->getAs<clang::RecordType>())
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

StructDeclare* ParameterDeclare::getStructDeclare() const
{
    if (auto param = llvm::dyn_cast<clang::ParmVarDecl>(decl))
    {
        if (auto type = param->getType()->getAs<clang::RecordType>())
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

FunctionDeclare::FunctionDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : Declare(decl, file_id, root)
{
    if (auto function = llvm::dyn_cast<clang::FunctionDecl>(decl))
    {
        for (auto parameter : function->parameters())
        {
            parameters.emplace_back(new ParameterDeclare(parameter, file_id, root));
        }
    }
}

FunctionDeclare::~FunctionDeclare()
{
    for (auto parameter : parameters) delete parameter;
    for (auto var : vars) delete var;
}

}