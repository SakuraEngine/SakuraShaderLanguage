#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

void HLSLShaderLibrary::atomicExtractStageOutputs(HLSLFlatStageOutput& hlslStage, TypeDeclare* declType, Declare* decl, const AnalysisStageOutput* Ana)
{
    auto& hlslOutput = hlslStage.fields.emplace_back();
    hlslOutput.ana = Ana;
    if (auto builtin = declType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute))
    {
        hlslOutput.type = StringToHLSLType(builtin->getBuiltinName().c_str());
    }
    hlslOutput.name = decl->getDecl()->getNameAsString();

    if (auto attrAttr = decl->findAttribute<AttributeAttribute>(kAttributeAttribute))
    {
        hlslOutput.index = 0u;
        hlslOutput.semantic = attrAttr->getSemantic();
        std::transform(hlslOutput.semantic.begin(), hlslOutput.semantic.end(), hlslOutput.semantic.begin(), ::toupper);
    }
    else if (auto svAttr = decl->findAttribute<SVAttribute>(kSVShaderAttribute))
    {
        hlslOutput.index = 0u;
        hlslOutput.semantic = "SV_" + svAttr->getSemantic();
        std::transform(hlslOutput.semantic.begin(), hlslOutput.semantic.end(), hlslOutput.semantic.begin(), ::toupper);
    }
    else
    {
        hlslOutput.index = 0u;
        hlslOutput.semantic = decl->getDecl()->getNameAsString();
    }
}

void HLSLShaderLibrary::recursiveExtractStageOutputs(HLSLFlatStageOutput& hlslStage, TypeDeclare* declType, Declare* decl, const AnalysisStageOutput* Ana)
{
    if (auto builtinType = llvm::dyn_cast<BuiltinDeclare>(declType))
    {
        atomicExtractStageOutputs(hlslStage, declType, decl, Ana);
    }
    else if (auto structType = llvm::dyn_cast<StructureDeclare>(declType))
    {
        for (auto field : structType->getFields())
        {
            auto fieldType = field->getTypeDeclare();
            if (auto builtin = fieldType ? fieldType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute) : nullptr)
            {
                atomicExtractStageOutputs(hlslStage, fieldType, field, Ana);
            }
            else
            {
                recursiveExtractStageOutputs(hlslStage, fieldType, field, Ana);
            }
        }
    }
}

void HLSLShaderLibrary::recursiveExtractStageOutputs(HLSLFlatStageOutput& hlslStage, const AnalysisStageOutput* Ana)
{
    if (auto paramType = Ana->as_param ? Ana->as_param->getTypeDeclare() : nullptr)
    {
        recursiveExtractStageOutputs(hlslStage, paramType, Ana->as_param, Ana);
    }
}

void HLSLShaderLibrary::extractStageOutputs()
{
    const auto& ana = f.getAnalysis();
    // extract
    for (auto stage : ana.stages)
    {
        auto& hlslStage = stage_outputs.emplace_back(&stage);
        for (auto output : stage.outputs)
        {
            recursiveExtractStageOutputs(hlslStage, &output);
        }
    }

    // remove duplicates
    for (auto& hlslStage : stage_outputs)
    {
        auto fields = hlslStage.getFields();
        llvm::SmallVector<HLSLOutput> unique;
        unique.reserve(fields.size());
        for (auto& hlslOutput : fields)
        {
            auto it = std::find_if(unique.begin(), unique.end(), 
            [&](const HLSLOutput& other) {
                return hlslOutput.semantic == other.semantic;
            });
            if (it == unique.end())
            {
                unique.emplace_back(hlslOutput);
            }
        }
        hlslStage.fields = std::move(unique);
    }
}

std::string HLSLShaderLibrary::serializeStageOutputs() const
{
    std::string serialized = "// Stage Outputs\n";
    auto newline = [&]() { serialized += "\n    "; };
    for (const auto& stage : stage_outputs)
    {
        serialized += "struct ";
        serialized += stage.getHLSLTypeName();
        serialized += " {";
        for (auto output : stage.getFields())
        {
            newline();
            serialized += HLSLTypeToString(output.type);
            serialized += " ";
            serialized += GetSemanticVarName(output.semantic.c_str());
            serialized += " : ";
            serialized += output.semantic;
            serialized += ";";
        }
        serialized += "\n};\n\n";
    }
    return serialized;
}

}