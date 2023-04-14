#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

HLSLFlatStageInput::HLSLFlatStageInput(const HLSLShaderLibrary* root, const AnalysisShaderStage* ana)
    : HLSLStruct(root), ana(ana)
{
    auto fname = ana->function->getDecl()->getQualifiedNameAsString();
    std::replace(fname.begin(), fname.end(), ':', '_');
    HLSLTypeName = fname + "_Inputs";
}

void HLSLStage::atomicExtractStageInputs(HLSLFlatStageInput& hlslStage, const TypeDeclare* declType, const Declare* decl, const AnalysisStageInput* Ana)
{
    if (auto svAttr = decl->findAttribute<SVAttribute>(kSVShaderAttribute)) return;

    auto& hlslInput = hlslStage.fields.emplace_back(root);
    hlslInput.ana = Ana;
    if (auto builtin = declType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute))
    {
        hlslInput.type = root->FindHLSLType(builtin->getBuiltinName().c_str());
    }
    hlslInput.name = decl->getDecl()->getNameAsString();
    if (auto attrAttr = decl->findAttribute<AttributeAttribute>(kAttributeAttribute))
    {
        hlslInput.index = 0u;
        hlslInput.semantic = attrAttr->getSemantic();
        std::transform(hlslInput.semantic.begin(), hlslInput.semantic.end(), hlslInput.semantic.begin(), ::toupper);
    }
    else
    {
        hlslInput.index = 0u;
        hlslInput.semantic = decl->getDecl()->getNameAsString();
    }
}

void HLSLStage::recursiveExtractStageInputs(HLSLFlatStageInput& hlslStage, const TypeDeclare* declType, const Declare* decl, const AnalysisStageInput* Ana)
{
    if (auto builtinType = llvm::dyn_cast<BuiltinDeclare>(declType))
    {
        atomicExtractStageInputs(hlslStage, builtinType, decl, Ana);
    }
    else if (auto structType = llvm::dyn_cast<StructureDeclare>(declType))
    {
        for (auto field : structType->getFields())
        {
            auto fieldType = field->getTypeDeclare();
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

void HLSLStage::recursiveExtractStageInputs(HLSLFlatStageInput& hlslStage, const AnalysisStageInput* Ana)
{
    if (auto decl = Ana->getAsDeclare())
    {
        recursiveExtractStageInputs(hlslStage, decl->getTypeDeclare(), decl, Ana);
    }
}

void HLSLStage::extractStageInputs()
{
    // extract
    for (auto input : s.inputs)
    {
        recursiveExtractStageInputs(stage_input, &input);
    }
    // remove duplicates
    {
        auto fields = stage_input.getFields();
        llvm::SmallVector<HLSLInput> unique;
        unique.reserve(fields.size());
        for (auto& hlslInput : fields)
        {
            auto it = std::find_if(unique.begin(), unique.end(), 
            [&](const HLSLInput& other) {
                return hlslInput.semantic == other.semantic;
            });
            if (it == unique.end())
            {
                unique.emplace_back(hlslInput);
            }
        }
        stage_input.fields = std::move(unique);
    }
}

std::string HLSLStage::serializeStageInputs() const
{
    std::string serialized = "";
    auto newline = [&]() { serialized += "\n    "; };
    {
        serialized += "struct ";
        serialized += stage_input.getHLSLTypeName();
        serialized += " {";
        for (auto input : stage_input.getFields())
        {
            newline();
            serialized += HLSLTypeToString(input.type);
            serialized += " ";
            serialized += input.name;
            serialized += " : ";
            serialized += input.semantic;
            serialized += ";";
        }
        serialized += "\n};\n\n";
    }
    return serialized;
}

}