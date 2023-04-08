#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"

namespace ssl::hlsl
{
const char* HLSLTypeToString(HLSLType type)
{
    switch (type)
    {
    case HLSLType::Float4: return "float4";
    case HLSLType::Float3: return "float3";
    case HLSLType::Float2: return "float2";
    case HLSLType::Float: return "float";
    case HLSLType::Int4: return "int4";
    case HLSLType::Int3: return "int3";
    case HLSLType::Int2: return "int2";
    case HLSLType::Int: return "int";
    case HLSLType::UInt: return "uint4";
    case HLSLType::UInt3: return "uint3";
    case HLSLType::UInt2: return "uint2";
    case HLSLType::Float4x4: return "float4x4";
    case HLSLType::Void: return "void";
    default: return "unknown";
    }
}

HLSLType StringToHLSLType(const char* type)
{
    if (strcmp(type, "float4") == 0) return HLSLType::Float4;
    if (strcmp(type, "float3") == 0) return HLSLType::Float3;
    if (strcmp(type, "float2") == 0) return HLSLType::Float2;
    if (strcmp(type, "float") == 0) return HLSLType::Float;
    if (strcmp(type, "int4") == 0) return HLSLType::Int4;
    if (strcmp(type, "int3") == 0) return HLSLType::Int3;
    if (strcmp(type, "int2") == 0) return HLSLType::Int2;
    if (strcmp(type, "int") == 0) return HLSLType::Int;
    if (strcmp(type, "uint4") == 0) return HLSLType::UInt4;
    if (strcmp(type, "uint3") == 0) return HLSLType::UInt3;
    if (strcmp(type, "uint2") == 0) return HLSLType::UInt2;
    if (strcmp(type, "float4x4") == 0) return HLSLType::Float4x4;
    if (strcmp(type, "void") == 0) return HLSLType::Void;
    if (strcmp(type, "struct") == 0) return HLSLType::Struct;
    return HLSLType::Void;
}

HLSLStage::HLSLStage(const AnalysisShaderStage* ana)
    : ana(ana)
{
    auto fname = ana->function->getDecl()->getNameAsString();
    HLSLTypeName = fname + "_Inputs";
}

HLSLShaderLibrary::HLSLShaderLibrary(const SourceFile& f, const HLSLOptions& options)
    : f(f)
{

}

void HLSLShaderLibrary::atomicExtractStageInputs(HLSLStage& hlslStage, StructDeclare* declType, Declare* decl, const AnalysisStageInput* Ana)
{
    auto& hlslInputs = hlslStage.inputs;
    auto& hlslInput = hlslInputs.all.emplace_back();
    hlslInput.ana = Ana;
    if (auto builtin = declType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute))
    {
        hlslInput.type = StringToHLSLType(builtin->getBuiltinName().c_str());
    }
    hlslInput.name = decl->getDecl()->getNameAsString();
    if (auto attrAttr = decl->findAttribute<AttributeAttribute>(kAttributeAttribute))
    {
        hlslInput.index = attrAttr->getAttributeIndex();
        hlslInput.semantic = attrAttr->getSemantic();
        std::transform(hlslInput.semantic.begin(), hlslInput.semantic.end(), hlslInput.semantic.begin(), ::toupper);
    }
    else
    {
        hlslInput.index = 0u;
        hlslInput.semantic = decl->getDecl()->getNameAsString();
    }
}

void HLSLShaderLibrary::recursiveExtractStageInputs(HLSLStage& hlslStage, StructDeclare* declType, Declare* decl, const AnalysisStageInput* Ana)
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

void HLSLShaderLibrary::recursiveExtractStageInputs(HLSLStage& hlslStage, const AnalysisStageInput* Ana)
{
    if (auto paramType = Ana->as_param ? Ana->as_param->getStructDeclare() : nullptr)
    {
        recursiveExtractStageInputs(hlslStage, paramType, Ana->as_param, Ana);
    }
}

void HLSLShaderLibrary::extractStageInputs()
{
    const auto& ana = f.getAnalysis();
    for (auto stage : ana.stages)
    {
        auto& hlslStage = stages.emplace_back(&stage);
        for (auto input : stage.inputs)
        {
            recursiveExtractStageInputs(hlslStage, &input);
        }
    }
}

void HLSLShaderLibrary::translate()
{
    extractStageInputs();
}

std::string HLSLShaderLibrary::serialize() const
{
    std::string result = "";
    std::string stageInputs = "";
    for (const auto& stage : stages)
    {
        stageInputs += "struct ";
        stageInputs += stage.getHLSLTypeName();
        stageInputs += " {";
        for (auto input : stage.inputs.all)
        {
            stageInputs += "\n    ";
            stageInputs += HLSLTypeToString(input.type);
            stageInputs += " ";
            stageInputs += input.name;
            stageInputs += " : ";
            stageInputs += input.semantic;
            stageInputs += ";";
        }
        stageInputs += "\n};\n\n";
    }
    result += stageInputs;
    return result;
}

std::string compile(const SourceFile& P, const HLSLOptions& options)
{
    HLSLShaderLibrary lib(P, options);
    lib.translate();
    return lib.serialize();
}
}