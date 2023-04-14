#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{
const char* HLSLTypeToString(HLSLType type)
{
    switch (type.primitiveType)
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
    case HLSLType::Struct: 
    {
        return type.structType->getHLSLTypeName();        
    }
    default: return "unknown";
    }
}

HLSLType HLSLShaderLibrary::FindHLSLType(const char* typeS) const
{
    if (strcmp(typeS, "float4") == 0) return HLSLType::Float4;
    if (strcmp(typeS, "float3") == 0) return HLSLType::Float3;
    if (strcmp(typeS, "float2") == 0) return HLSLType::Float2;
    if (strcmp(typeS, "float") == 0) return HLSLType::Float;
    if (strcmp(typeS, "int4") == 0) return HLSLType::Int4;
    if (strcmp(typeS, "int3") == 0) return HLSLType::Int3;
    if (strcmp(typeS, "int2") == 0) return HLSLType::Int2;
    if (strcmp(typeS, "int") == 0) return HLSLType::Int;
    if (strcmp(typeS, "uint4") == 0) return HLSLType::UInt4;
    if (strcmp(typeS, "uint3") == 0) return HLSLType::UInt3;
    if (strcmp(typeS, "uint2") == 0) return HLSLType::UInt2;
    if (strcmp(typeS, "float4x4") == 0) return HLSLType::Float4x4;
    if (strcmp(typeS, "void") == 0) return HLSLType::Void;
    return HLSLType::Struct;
}

HLSLType HLSLShaderLibrary::FindHLSLType(const TypeDeclare* type) const
{
    auto typeS = type->getDecl()->getNameAsString();
    if (strcmp(typeS.c_str(), "float4") == 0) return HLSLType::Float4;
    if (strcmp(typeS.c_str(), "float3") == 0) return HLSLType::Float3;
    if (strcmp(typeS.c_str(), "float2") == 0) return HLSLType::Float2;
    if (strcmp(typeS.c_str(), "float") == 0) return HLSLType::Float;
    if (strcmp(typeS.c_str(), "int4") == 0) return HLSLType::Int4;
    if (strcmp(typeS.c_str(), "int3") == 0) return HLSLType::Int3;
    if (strcmp(typeS.c_str(), "int2") == 0) return HLSLType::Int2;
    if (strcmp(typeS.c_str(), "int") == 0) return HLSLType::Int;
    if (strcmp(typeS.c_str(), "uint4") == 0) return HLSLType::UInt4;
    if (strcmp(typeS.c_str(), "uint3") == 0) return HLSLType::UInt3;
    if (strcmp(typeS.c_str(), "uint2") == 0) return HLSLType::UInt2;
    if (strcmp(typeS.c_str(), "float4x4") == 0) return HLSLType::Float4x4;
    if (strcmp(typeS.c_str(), "void") == 0) return HLSLType::Void;
    {
        // find in structs
        for (const auto& s : structures)
        {
            if (s->decl == type) return HLSLType(s); 
        }
    }
    return HLSLType::Struct;
}

std::string GetSemanticVarName(const FunctionDeclare* func, const char* semantic)
{
    auto fname = func->getDecl()->getQualifiedNameAsString();
    std::replace(fname.begin(), fname.end(), ':', '_');
    return fname + "_" + semantic;
}

HLSLShaderLibrary::HLSLShaderLibrary(const SourceFile& f, const HLSLOptions& options)
    : f(f)
{

}

HLSLShaderLibrary::~HLSLShaderLibrary()
{
    for (auto s : structures)
    {
        delete s;
    }
}

void HLSLShaderLibrary::translate()
{
    makeStructures();
    makeFunctions();

    for (auto& stage : stages)
        stage.extractStageInputs();
    for (auto& stage : stages)
        stage.extractStageOutputs();
    for (auto& stage : stages)
        stage.extractStageSVs();
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
    serialized += serializeFunctions();

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