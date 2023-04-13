#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

void HLSLShaderLibrary::atomicExtractStageOutputs(HLSLFlatStageOutput& hlslStage, const TypeDeclare* declType, const Declare* decl, const AnalysisStageOutput* Ana)
{
    HLSLOutput newOutput = {};
    newOutput.ana = Ana;
    if (auto builtin = declType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute))
    {
        newOutput.type = StringToHLSLType(builtin->getBuiltinName().c_str());
    }
    newOutput.name = decl->getDecl()->getNameAsString();

    if (auto attrAttr = decl->findAttribute<AttributeAttribute>(kAttributeAttribute))
    {
        newOutput.index = 0u;
        newOutput.semantic = attrAttr->getSemantic();
        std::transform(newOutput.semantic.begin(), newOutput.semantic.end(), newOutput.semantic.begin(), ::toupper);
        hlslStage.fields.emplace_back(newOutput);
    }
    else if (auto svAttr = decl->findAttribute<SVAttribute>(kSVShaderAttribute))
    {
        newOutput.index = 0u;
        newOutput.semantic = "SV_" + svAttr->getSemantic();
        std::transform(newOutput.semantic.begin(), newOutput.semantic.end(), newOutput.semantic.begin(), ::toupper);
    }
    else
    {
        newOutput.index = 0u;
        newOutput.semantic = decl->getDecl()->getNameAsString();
        hlslStage.fields.emplace_back(newOutput);
    }
}

void HLSLShaderLibrary::recursiveExtractStageOutputs(HLSLFlatStageOutput& hlslStage, const TypeDeclare* declType, const Declare* decl, const AnalysisStageOutput* Ana)
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
    if (auto decl = Ana->getAsDeclare())
    {
        recursiveExtractStageOutputs(hlslStage, decl->getTypeDeclare(), decl, Ana);
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
    std::string serialized = "// 2. Stage Outputs\n";
    auto newline = [&]() { serialized += "\n    "; };
    for (const auto& stage : stage_outputs)
    {
        const auto& fields = stage.getFields();
        if (!fields.empty())
        {
            serialized += "struct ";
            serialized += stage.getHLSLTypeName();
            serialized += " {";
            for (auto output : fields)
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
        else
        {
            serialized += "#define ";
            serialized += stage.getHLSLTypeName();
            serialized += " void\n\n";
        }
    }
    return serialized;
}

}