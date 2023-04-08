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

struct HLSLInput
{
    const AnalysisStageInput* ana;
    uint64_t index;
    std::string semantic;
    HLSLType type;
    std::string name;
};

struct HLSLStageInput
{
    llvm::SmallVector<HLSLInput> all;
};

struct HLSLStruct
{
    const char* getHLSLTypeName() const { return HLSLTypeName.c_str(); }

protected:
    std::string HLSLTypeName;
};

struct HLSLStage : public HLSLStruct
{
    HLSLStage(const AnalysisShaderStage* ana);

    HLSLStageInput inputs;
protected:
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

    void atomicExtractStageInputs(HLSLStage& hlslStage, StructDeclare* declType, Declare* decl, const AnalysisStageInput* Ana);
    void recursiveExtractStageInputs(HLSLStage& hlslStage, StructDeclare* declType, Declare* decl, const AnalysisStageInput* Ana);
    void recursiveExtractStageInputs(HLSLStage& hlslStage, const AnalysisStageInput* Ana);
    void extractStageInputs();

    const SourceFile& f;
    llvm::SmallVector<HLSLStage> stages;
};

std::string compile(const SourceFile& P, const HLSLOptions& options = {});
}
} // namespace ssl