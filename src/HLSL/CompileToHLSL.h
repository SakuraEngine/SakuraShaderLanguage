#pragma once
#include "./../Source.h"

namespace ssl
{
namespace hlsl
{
struct HLSLElement
{
    HLSLElement() = default;
    HLSLElement(const struct HLSLShaderLibrary* root) : root(root) {}
    virtual ~HLSLElement() = default;
protected:
    const struct HLSLShaderLibrary* root = nullptr;
};

struct HLSLType 
{
    enum Primitive : uint32_t
    {
        Unknown,
        Float, Float2, Float3, Float4,
        Int, Int2, Int3, Int4,
        UInt, UInt2, UInt3, UInt4,
        Float4x4,
        Void,
        Struct
    };

    HLSLType(Primitive primitiveType) : primitiveType(primitiveType) {}
    HLSLType(const struct HLSLStruct* structType) : primitiveType(Struct), structType(structType) {}

    bool operator==(Primitive primitiveType) const { return this->primitiveType == primitiveType; }

    Primitive primitiveType = Primitive::Unknown;
    const struct HLSLStruct* structType = nullptr;
};

const char* HLSLTypeToString(HLSLType type);
std::string GetSemanticVarName(const FunctionDeclare* func, const char* semantic);

struct HLSLParameter: public HLSLElement
{
    HLSLParameter() = default;
    HLSLParameter(const struct HLSLShaderLibrary* root, const TypeDeclare* declType, const ParameterDeclare* decl, struct HLSLFunction* funcType);

    HLSLType getType() const { return type; }
    const char* getName() const { return name.c_str(); }
    bool isSystemValue() const;
    const char* getSemantic() const { return semantic.c_str(); }
    
protected:
    const TypeDeclare* declType = nullptr;
    const ParameterDeclare* decl = nullptr;
    struct HLSLFunction* funcType = nullptr;

    HLSLType type = HLSLType::Unknown;
    std::string name = "";
    std::string semantic = "";
};

struct HLSLField: public HLSLElement
{
    friend struct HLSLStage;
    friend struct HLSLShaderLibrary;

    HLSLField() = default;
    HLSLField(const HLSLShaderLibrary* root) : HLSLElement(root) {}
    HLSLField(const HLSLShaderLibrary* root, const TypeDeclare* declType, const FieldDeclare* decl);

    HLSLType getType() const { return type; }
    const char* getName() const { return name.c_str(); }

protected:
    const TypeDeclare* declType = nullptr;
    const FieldDeclare* decl = nullptr;

    HLSLType type = HLSLType::Unknown;
    std::string name = "";
};

struct HLSLSemanticField : public HLSLField
{
    HLSLSemanticField(const HLSLShaderLibrary* root) : HLSLField(root) {}

    uint64_t index;
    std::string semantic;
};

struct HLSLSystemValue : public HLSLSemanticField
{
    HLSLSystemValue(const HLSLShaderLibrary* root) : HLSLSemanticField(root) {}

    const AnalysisSystemValues* ana;
};

struct HLSLInput : public HLSLSemanticField
{
    HLSLInput(const HLSLShaderLibrary* root) : HLSLSemanticField(root) {}

    const AnalysisStageInput* ana;
};

struct HLSLOutput : public HLSLSemanticField
{
    HLSLOutput(const HLSLShaderLibrary* root) : HLSLSemanticField(root) {}

    const AnalysisStageOutput* ana;
};

struct HLSLStruct: public HLSLElement
{
    HLSLStruct(const HLSLShaderLibrary* root) : HLSLElement(root) {}
    virtual ~HLSLStruct() = default;
    const char* getHLSLTypeName() const { return HLSLTypeName.c_str(); }

protected:
    std::string HLSLTypeName;
};

struct HLSLPlainStruct : public HLSLStruct
{
    friend struct HLSLStage;
    friend struct HLSLShaderLibrary;
    HLSLPlainStruct(const HLSLShaderLibrary* root, const StructureDeclare* decl);

    std::span<const HLSLField> getFields() const { return fields; }
protected:
    llvm::SmallVector<HLSLField> fields;
    const StructureDeclare* decl = nullptr;
};

struct HLSLFlatStageInput : public HLSLStruct
{
    friend struct HLSLStage;
    friend struct HLSLShaderLibrary;
    HLSLFlatStageInput(const HLSLShaderLibrary* root, const AnalysisShaderStage* ana);

    std::span<const HLSLInput> getFields() const { return fields; }
protected:
    llvm::SmallVector<HLSLInput> fields;
    const AnalysisShaderStage* ana;
};

struct HLSLFlatStageOutput : public HLSLStruct
{
    friend struct HLSLStage;
    friend struct HLSLShaderLibrary;
    HLSLFlatStageOutput(const HLSLShaderLibrary* root, const AnalysisShaderStage* ana);

    std::span<const HLSLOutput> getFields() const { return fields; }
protected:
    llvm::SmallVector<HLSLOutput> fields;
    const AnalysisShaderStage* ana;
};

struct HLSLFlatSVs : public HLSLStruct
{
    friend struct HLSLStage;
    friend struct HLSLShaderLibrary;
    HLSLFlatSVs(const HLSLShaderLibrary* root, const AnalysisShaderStage* ana);

    std::span<const HLSLSystemValue> getFields() const { return fields; }
protected:
    llvm::SmallVector<HLSLSystemValue> fields;
    const AnalysisShaderStage* ana;
};

struct HLSLFunction : public HLSLElement
{
    friend struct HLSLStage;
    friend struct HLSLShaderLibrary;
    HLSLFunction() = default;
    HLSLFunction(const HLSLShaderLibrary* root, const FunctionDeclare* decl);
    
protected:
    llvm::SmallVector<HLSLParameter> parameters;
    const FunctionDeclare* declare = nullptr;
};

struct HLSLStage : public HLSLElement
{
public:
    HLSLStage(const HLSLShaderLibrary* root, const AnalysisShaderStage& ana, const HLSLFunction& func) 
        : HLSLElement(root), s(ana), func(func), 
          stage_input(root, &ana), stage_output(root, &ana), 
          stage_SVs(root, &ana) 
    {

    }

    // 1. extract all stage inputs
    void extractStageInputs();
    std::string serializeStageInputs() const;
    // 2. extract all stage outputs
    void extractStageOutputs();
    std::string serializeStageOutputs() const;
    // 3. extract all stage sysval access
    void extractStageSVs();
    std::string serializeStageSVs() const;
    std::string makeStageSVsSignature() const;

    // 6. make assemblers
    void makeAssemblers();
    std::string serializeAssemblers() const;

protected:
    // 1.
    void atomicExtractStageInputs(HLSLFlatStageInput& hlslStage, const TypeDeclare* declType, const Declare* decl, const AnalysisStageInput* Ana);
    void recursiveExtractStageInputs(HLSLFlatStageInput& hlslStage, const TypeDeclare* declType, const Declare* decl, const AnalysisStageInput* Ana);
    void recursiveExtractStageInputs(HLSLFlatStageInput& hlslStage, const AnalysisStageInput* Ana);
    // 2.
    void atomicExtractStageOutputs(HLSLFlatStageOutput& hlslStage, const TypeDeclare* declType, const Declare* decl, const AnalysisStageOutput* Ana);
    void recursiveExtractStageOutputs(HLSLFlatStageOutput& hlslStage, const TypeDeclare* declType, const Declare* decl, const AnalysisStageOutput* Ana);
    void recursiveExtractStageOutputs(HLSLFlatStageOutput& hlslStage, const AnalysisStageOutput* Ana);
    // 3.
    void atomicExtractStageSVs(HLSLFlatSVs& hlslSV, const TypeDeclare* declType, const Declare* decl, const AnalysisSystemValues* Ana);
    void recursiveExtractStageSVs(HLSLFlatSVs& hlslSV, const TypeDeclare* declType, const Declare* decl, const AnalysisSystemValues* Ana);
    void recursiveExtractStageSVs(HLSLFlatSVs& hlslSV, const AnalysisSystemValues* Ana);

    const HLSLFunction& func;
    const AnalysisShaderStage& s;
    HLSLFlatStageInput stage_input;
    HLSLFlatStageOutput stage_output;
    HLSLFlatSVs stage_SVs;
};

struct HLSLOptions
{
    bool CommentSourceLine = false;
};

struct HLSLShaderLibrary
{
    HLSLShaderLibrary(const SourceFile& f, const HLSLOptions& options);
    ~HLSLShaderLibrary();

    std::string serialize() const;
    void translate();

    // 4. make structures
    void makeStructures();
    std::string serializeStructures() const;
    // 5. make functions
    void makeFunctions();
    std::string serializeFunctions() const;

    HLSLType FindHLSLType(const TypeDeclare* type) const;
    HLSLType FindHLSLType(const char* type) const;

    const SourceFile& f;
    llvm::SmallVector<HLSLPlainStruct*> structures;
    llvm::SmallVector<HLSLFunction, 8> functions;
    llvm::SmallVector<HLSLStage, 2> stages;
};

std::string compile(const SourceFile& P, const HLSLOptions& options = {});
}
} // namespace ssl