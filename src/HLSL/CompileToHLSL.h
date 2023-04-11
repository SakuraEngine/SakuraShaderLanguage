#pragma once
#include "./../Source.h"

namespace ssl
{
namespace hlsl
{

enum class HLSLType : uint32_t
{
    Unknown,
    Float, Float2, Float3, Float4,
    Int, Int2, Int3, Int4,
    UInt, UInt2, UInt3, UInt4,
    Float4x4,
    Void,
    Struct
};

const char* HLSLTypeToString(HLSLType type);
HLSLType StringToHLSLType(const char* type);
std::string GetSemanticVarName(const char* semantic);

struct HLSLField
{
    friend struct HLSLShaderLibrary;
    HLSLField() = default;
    HLSLField(TypeDeclare* declType, const FieldDeclare* decl, struct HLSLStruct* structType);

protected:
    const TypeDeclare* declType = nullptr;
    const FieldDeclare* decl = nullptr;
    struct HLSLStruct* structType = nullptr;

    HLSLType type = HLSLType::Unknown;
    std::string name = "";
};

struct HLSLInput : public HLSLField
{
    const AnalysisStageInput* ana;
    uint64_t index;
    std::string semantic;
};

struct HLSLOutput : public HLSLField
{
    const AnalysisStageOutput* ana;
    uint64_t index;
    std::string semantic;
};

struct HLSLStruct
{
    virtual ~HLSLStruct() = default;
    const char* getHLSLTypeName() const { return HLSLTypeName.c_str(); }

protected:
    std::string HLSLTypeName;
};

struct HLSLPlainStruct : public HLSLStruct
{
    friend struct HLSLShaderLibrary;
    HLSLPlainStruct(const StructureDeclare* decl);

    std::span<const HLSLField> getFields() const { return fields; }
protected:
    llvm::SmallVector<HLSLField> fields;
    const StructureDeclare* decl = nullptr;
};

struct HLSLFlatStageInput : public HLSLStruct
{
    friend struct HLSLShaderLibrary;
    HLSLFlatStageInput(const AnalysisShaderStage* ana);

    std::span<const HLSLInput> getFields() const { return fields; }
protected:
    llvm::SmallVector<HLSLInput> fields;
    const AnalysisShaderStage* ana;
};

struct HLSLFlatStageOutput : public HLSLStruct
{
    friend struct HLSLShaderLibrary;
    HLSLFlatStageOutput(const AnalysisShaderStage* ana);

    std::span<const HLSLOutput> getFields() const { return fields; }
protected:
    llvm::SmallVector<HLSLOutput> fields;
    const AnalysisShaderStage* ana;
};

struct HLSLFunction
{

};

struct HLSLOptions
{
    bool CommentSourceLine = false;
};

struct HLSLShaderLibrary
{
    HLSLShaderLibrary(const SourceFile& f, const HLSLOptions& options);

    std::string serialize() const;
    void translate();

    // 1. extract all stage inputs
    void extractStageInputs();
    std::string serializeStageInputs() const;
    // 2. extract all stage outputs
    void extractStageOutputs();
    std::string serializeStageOutputs() const;
    // 3. make structures
    void makeStructures();
    std::string serializeStructures() const;
    // 4. make functions
    void makeFunctions();
    std::string serializeFunctions() const;

protected:
    // 1.
    void atomicExtractStageInputs(HLSLFlatStageInput& hlslStage, TypeDeclare* declType, Declare* decl, const AnalysisStageInput* Ana);
    void recursiveExtractStageInputs(HLSLFlatStageInput& hlslStage, TypeDeclare* declType, Declare* decl, const AnalysisStageInput* Ana);
    void recursiveExtractStageInputs(HLSLFlatStageInput& hlslStage, const AnalysisStageInput* Ana);
    // 2.
    void atomicExtractStageOutputs(HLSLFlatStageOutput& hlslStage, TypeDeclare* declType, Declare* decl, const AnalysisStageOutput* Ana);
    void recursiveExtractStageOutputs(HLSLFlatStageOutput& hlslStage, TypeDeclare* declType, Declare* decl, const AnalysisStageOutput* Ana);
    void recursiveExtractStageOutputs(HLSLFlatStageOutput& hlslStage, const AnalysisStageOutput* Ana);
    // 3.

    const SourceFile& f;
    llvm::SmallVector<HLSLFlatStageInput> stage_inputs;
    llvm::SmallVector<HLSLFlatStageOutput> stage_outputs;
    llvm::SmallVector<HLSLPlainStruct> structures;
};

std::string compile(const SourceFile& P, const HLSLOptions& options = {});
}
} // namespace ssl