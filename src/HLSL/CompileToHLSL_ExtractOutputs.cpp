#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

HLSLFlatStageOutput::HLSLFlatStageOutput(const HLSLShaderLibrary* root, const AnalysisShaderStage* ana)
    : HLSLStruct(root), ana(ana)
{
    auto fname = ana->function->getDecl()->getQualifiedNameAsString();
    std::replace(fname.begin(), fname.end(), ':', '_');
    HLSLTypeName = fname + "_Outputs";
}

void HLSLStage::atomicExtractStageOutputs(HLSLFlatStageOutput& hlslStage, const TypeDeclare* declType, const Declare* decl, const AnalysisStageOutput* Ana)
{
    if (auto svAttr = decl->findAttribute<SVAttribute>(kSVShaderAttribute)) return;

    auto& newOutput = hlslStage.fields.emplace_back(root);
    newOutput.ana = Ana;
    if (auto builtin = declType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute))
    {
        newOutput.type = root->FindHLSLType(builtin->getBuiltinName().c_str());
    }
    newOutput.name = decl->getDecl()->getNameAsString();

    if (auto attrAttr = decl->findAttribute<AttributeAttribute>(kAttributeAttribute))
    {
        newOutput.index = 0u;
        newOutput.semantic = attrAttr->getSemantic();
        std::transform(newOutput.semantic.begin(), newOutput.semantic.end(), newOutput.semantic.begin(), ::toupper);
    }
    else
    {
        newOutput.index = 0u;
        newOutput.semantic = decl->getDecl()->getNameAsString();
    }
}

void HLSLStage::recursiveExtractStageOutputs(HLSLFlatStageOutput& hlslStage, const TypeDeclare* declType, const Declare* decl, const AnalysisStageOutput* Ana)
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

void HLSLStage::recursiveExtractStageOutputs(HLSLFlatStageOutput& hlslStage, const AnalysisStageOutput* Ana)
{
    if (auto decl = Ana->getAsDeclare())
    {
        recursiveExtractStageOutputs(hlslStage, decl->getTypeDeclare(), decl, Ana);
    }
}

void HLSLStage::extractStageOutputs()
{
    // extract
    for (auto output : s.outputs)
    {
        recursiveExtractStageOutputs(stage_output, &output);
    }

    // remove duplicates
    {
        auto fields = stage_output.getFields();
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
        stage_output.fields = std::move(unique);
    }
}

std::string HLSLStage::serializeStageOutputs() const
{
    std::string serialized = "";
    auto newline = [&]() { serialized += "\n    "; };
    const auto& fields = stage_output.getFields();
    if (!fields.empty())
    {
        serialized += "struct ";
        serialized += stage_output.getHLSLTypeName();
        serialized += " {";
        for (auto output : fields)
        {
            newline();
            serialized += HLSLTypeToString(output.type);
            serialized += " ";
            serialized += output.name;
            serialized += " : ";
            serialized += output.semantic;
            serialized += ";";
        }
        serialized += "\n};\n\n";
    }
    else
    {
        serialized += "#define ";
        serialized += stage_output.getHLSLTypeName();
        serialized += " void\n\n";
    }
    return serialized;
}

}