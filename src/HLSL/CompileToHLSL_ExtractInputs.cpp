#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

void HLSLShaderLibrary::atomicExtractStageInputs(HLSLFlatStageInput& hlslStage, StructDeclare* declType, Declare* decl, const AnalysisStageInput* Ana)
{
    auto& hlslInput = hlslStage.fields.emplace_back();
    hlslInput.ana = Ana;
    if (auto builtin = declType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute))
    {
        hlslInput.type = StringToHLSLType(builtin->getBuiltinName().c_str());
    }
    hlslInput.name = decl->getDecl()->getNameAsString();
    if (auto attrAttr = decl->findAttribute<AttributeAttribute>(kAttributeAttribute))
    {
        hlslInput.index = 0u;
        hlslInput.semantic = attrAttr->getSemantic();
        std::transform(hlslInput.semantic.begin(), hlslInput.semantic.end(), hlslInput.semantic.begin(), ::toupper);
    }
    else if (auto svAttr = decl->findAttribute<SVAttribute>(kSVShaderAttribute))
    {
        hlslInput.index = 0u;
        hlslInput.semantic = "SV_" + svAttr->getSemantic();
        std::transform(hlslInput.semantic.begin(), hlslInput.semantic.end(), hlslInput.semantic.begin(), ::toupper);
    }
    else
    {
        hlslInput.index = 0u;
        hlslInput.semantic = decl->getDecl()->getNameAsString();
    }
}

void HLSLShaderLibrary::recursiveExtractStageInputs(HLSLFlatStageInput& hlslStage, StructDeclare* declType, Declare* decl, const AnalysisStageInput* Ana)
{
    if (auto builtin = declType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute))
    {
        atomicExtractStageInputs(hlslStage, declType, decl, Ana);
    }
    else
    {
        for (auto field : declType->getFields())
        {
            auto fieldType = field->getStructDeclare();
            if (auto builtin = fieldType ? fieldType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute) : nullptr)
            {
                atomicExtractStageInputs(hlslStage, fieldType, field, Ana);
            }
            else
            {
                recursiveExtractStageInputs(hlslStage, fieldType, field, Ana);
            }
        }
    }
}

void HLSLShaderLibrary::recursiveExtractStageInputs(HLSLFlatStageInput& hlslStage, const AnalysisStageInput* Ana)
{
    if (auto paramType = Ana->as_param ? Ana->as_param->getStructDeclare() : nullptr)
    {
        recursiveExtractStageInputs(hlslStage, paramType, Ana->as_param, Ana);
    }
}

void HLSLShaderLibrary::extractStageInputs()
{
    const auto& ana = f.getAnalysis();
    // extract
    for (auto stage : ana.stages)
    {
        auto& hlslStage = stages.emplace_back(&stage);
        for (auto input : stage.inputs)
        {
            recursiveExtractStageInputs(hlslStage, &input);
        }
    }
    // remove duplicates
    for (auto& hlslStage : stages)
    {
        auto fields = hlslStage.getFields();
        llvm::SmallVector<HLSLInput> unique;
        unique.reserve(fields.size());
        for (auto& hlslInput : fields)
        {
            auto it = std::find_if(unique.begin(), unique.end(), 
            [&](const HLSLInput& other) {
                return hlslInput.semantic == other.semantic;
            });
            if (it == unique.end())
            {
                unique.emplace_back(hlslInput);
            }
        }
        hlslStage.fields = std::move(unique);
    }
}

std::string HLSLShaderLibrary::serializeStageInputs() const
{
    std::string serialized = "";
    auto newline = [&]() { serialized += "\n    "; };
    for (const auto& stage : stages)
    {
        serialized += "struct ";
        serialized += stage.getHLSLTypeName();
        serialized += " {";
        for (auto input : stage.getFields())
        {
            newline();
            serialized += HLSLTypeToString(input.type);
            serialized += " ";
            serialized += GetSemanticVarName(input.semantic.c_str());
            serialized += " : ";
            serialized += input.semantic;
            serialized += ";";
        }
        serialized += "\n};\n\n";
    }
    return serialized;
}

}