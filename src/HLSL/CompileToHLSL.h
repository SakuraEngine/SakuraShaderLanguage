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
    HLSLField(StructDeclare* declType, const FieldDeclare* decl, struct HLSLStruct* structType);

protected:
    const StructDeclare* declType = nullptr;
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
    HLSLPlainStruct(const StructDeclare* decl);

    std::span<const HLSLField> getFields() const { return fields; }

protected:
    llvm::SmallVector<HLSLField> fields;
    const StructDeclare* decl = nullptr;
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
    // 2. make structures
    void makeStructures();
    std::string serializeStructures() const;

protected:
    // 1.
    void atomicExtractStageInputs(HLSLFlatStageInput& hlslStage, StructDeclare* declType, Declare* decl, const AnalysisStageInput* Ana);
    void recursiveExtractStageInputs(HLSLFlatStageInput& hlslStage, StructDeclare* declType, Declare* decl, const AnalysisStageInput* Ana);
    void recursiveExtractStageInputs(HLSLFlatStageInput& hlslStage, const AnalysisStageInput* Ana);
    // 2.

    const SourceFile& f;
    llvm::SmallVector<HLSLFlatStageInput> stages;
    llvm::SmallVector<HLSLPlainStruct> structures;
};

std::string compile(const SourceFile& P, const HLSLOptions& options = {});
}
} // namespace ssl