#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

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

std::string GetSemanticVarName(const char* semantic)
{
    return std::string("__ssl__") + semantic;
}

std::string GetSemanticVarName(FunctionDeclare* func, const char* semantic)
{
    auto fname = func->getDecl()->getQualifiedNameAsString();
    std::replace(fname.begin(), fname.end(), ':', '_');
    return fname + "_" + semantic;
}

HLSLField::HLSLField(const TypeDeclare* declType, const FieldDeclare* decl, struct HLSLStruct* structType)
    : declType(declType), decl(decl), structType(structType) 
{
    type = StringToHLSLType(declType->getDecl()->getNameAsString().c_str());
    name = decl->getDecl()->getNameAsString();
}

HLSLPlainStruct::HLSLPlainStruct(const StructureDeclare* decl)
    : decl(decl)
{
    auto fullName = decl->getDecl()->getQualifiedNameAsString();
    // replace :: to __
    std::replace(fullName.begin(), fullName.end(), ':', '_');
    HLSLTypeName = fullName;
}

HLSLFlatStageInput::HLSLFlatStageInput(const AnalysisShaderStage* ana)
    : ana(ana)
{
    auto fname = ana->function->getDecl()->getQualifiedNameAsString();
    std::replace(fname.begin(), fname.end(), ':', '_');
    HLSLTypeName = fname + "_Inputs";
}

HLSLFlatStageOutput::HLSLFlatStageOutput(const AnalysisShaderStage* ana)
    : ana(ana)
{
    auto fname = ana->function->getDecl()->getQualifiedNameAsString();
    std::replace(fname.begin(), fname.end(), ':', '_');
    HLSLTypeName = fname + "_Outputs";
}

HLSLFlatSVs::HLSLFlatSVs(const AnalysisShaderStage* ana)
    : ana(ana)
{
    auto fname = ana->function->getDecl()->getQualifiedNameAsString();
    std::replace(fname.begin(), fname.end(), ':', '_');
    HLSLTypeName = fname + "_SVs";
}

HLSLShaderLibrary::HLSLShaderLibrary(const SourceFile& f, const HLSLOptions& options)
    : f(f)
{

}

void HLSLShaderLibrary::translate()
{
    for (const auto& s : f.getAnalysis().stages)
        stages.emplace_back(s);

    for (auto& stage : stages)
        stage.extractStageInputs();
    for (auto& stage : stages)
        stage.extractStageOutputs();
    for (auto& stage : stages)
        stage.extractStageSVs();
    
    makeStructures();
    
    for (auto& stage : stages)
        stage.makeFunctions();
    for (auto& stage : stages)
        stage.makeAssemblers();
}

std::string HLSLShaderLibrary::serialize() const
{
    std::string serialized = "";
    serialized += "// 1. Stage Inputs\n";
    for (auto& stage : stages)
        serialized += stage.serializeStageInputs();

    serialized += "// 2. Stage Outputs\n";
    for (auto& stage : stages)
        serialized += stage.serializeStageOutputs();

    serialized += "// 3. SystemValue accesses\n";
    for (auto& stage : stages)
        serialized += stage.serializeStageSVs();

    serialized += "// 4. HLSL structures\n";
    serialized += serializeStructures();

    serialized += "// 5. HLSL functions\n";
    for (auto& stage : stages)
        serialized += stage.serializeFunctions();

    serialized += "// 6. HLSL stage input/output assemblers\n";
    for (auto& stage : stages)
        serialized += stage.serializeAssemblers();

    return serialized;
}

std::string compile(const SourceFile& P, const HLSLOptions& options)
{
    HLSLShaderLibrary lib(P, options);
    lib.translate();
    return lib.serialize();
}
}