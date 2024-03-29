#include "./../Source.h"
#include "./../Attributes/ShaderAttr.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Stmt.h"

#include <iostream>

namespace ssl
{
const char* getAccessTypeString(AccessType acess)
{
    switch (acess)
    {
        case kAccessReading: return "in";
        case kAccessWriting: return "out";
        case kAccessReadWrite: return "inout";
        default: return "unknown";
    }
}

SourceFile::~SourceFile()
{
    for (auto function : functions)
        delete function;
    for (auto type : types) 
        delete type;
}

void SourceFile::analyze()
{
    // 1. Iterate and analyze all stages
    for (auto function : functions)
    for (auto attr : function->findAttributes(kStageShaderAttribute))
    {
        // 1.0 analyze function signature
        function->analyze(this);
        auto& newStage = analysis.stages.emplace_back();
        newStage.function = function;
        newStage.stage_attr = attr;

        // 1.1 extract system values
        for (auto param : function->getParameters())
        {
            auto sv_attrs = param->findAttributes(kSVShaderAttribute);
            for (auto sv_attr : sv_attrs)
            {
                newStage.svs.emplace_back(param, function->getAccessType(param));
            }
        }
        for (auto var : function->getOutterVars())
        {
            auto sv_attrs = var->findAttributes(kSVShaderAttribute);
            for (auto sv_attr : sv_attrs)
            {
                newStage.svs.emplace_back(
                    var, function->getAccessType(var)
                );
            }
        }

        // 1.2 generate input/output traits
        // analyze inputs/outputs with function signature params
        for (auto param : function->getParameters())
        {
            auto sv_attrs = param->findAttributes(kSVShaderAttribute);
            if (!sv_attrs.empty()) continue;
            if (param->isStageInput()) newStage.inputs.emplace_back(param);            
            if (param->isStageOutput()) newStage.outputs.emplace_back(param);            
        }
        for (auto var : function->getOutterVars())
        {
            auto sv_attrs = var->findAttributes(kSVShaderAttribute);
            if (!sv_attrs.empty()) continue;
            if (var->isStageInput()) newStage.inputs.emplace_back(var);            
            if (var->isStageOutput()) newStage.outputs.emplace_back(var);            
        }

        // 1.3 retval
        if (auto retType = function->getReturnType())
        {
            newStage.outputs.emplace_back(retType);
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

struct TypeDeclare* SourceFile::find(clang::RecordDecl* decl) const
{
    for (auto type : types)
    {
        if (type->getDecl() == decl)
        {
            return type;
        }
    }
    return nullptr;
}

struct VarDeclare* SourceFile::find(clang::ValueDecl* decl) const
{
    for (auto var : vars)
    {
        if (var->getDecl() == decl)
        {
            return var;
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
    if (decl)
    {
        root->declLocs[decl] = file_id;
    }
}

TypedDeclare::TypedDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : Declare(decl, file_id, root)
{

}


const ShaderAttribute* Declare::findAttribute(ShaderAttributeKind kind) const
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

const llvm::SmallVector<ShaderAttribute*> Declare::findAttributes(ShaderAttributeKind kind) const
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

const TypeDeclare* FieldDeclare::getTypeDeclare() const
{
    if (auto field = llvm::dyn_cast<clang::FieldDecl>(decl))
    {
        auto type = field->getType()->getAs<clang::RecordType>();
        // decay
        if (!type)
            type = field->getType()->getPointeeType()->getAs<clang::RecordType>();
        if (!type) 
            type = field->getType().getNonReferenceType()->getAs<clang::RecordType>();
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