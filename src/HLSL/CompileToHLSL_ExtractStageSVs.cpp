#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

HLSLFlatSVs::HLSLFlatSVs(const HLSLShaderLibrary* root, const AnalysisShaderStage* ana)
    : HLSLStruct(root), ana(ana)
{
    auto fname = ana->function->getDecl()->getQualifiedNameAsString();
    std::replace(fname.begin(), fname.end(), ':', '_');
    HLSLTypeName = fname + "_SVs";
}

void HLSLStage::atomicExtractStageSVs(HLSLFlatSVs& hlslSV, const TypeDeclare* declType, const Declare* decl, const AnalysisSystemValues* Ana)
{
    HLSLSystemValue& newSV = hlslSV.fields.emplace_back(root);
    newSV.ana = Ana;
    if (auto builtin = declType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute))
    {
        newSV.type = root->FindHLSLType(builtin->getBuiltinName().c_str());
    }
    newSV.name = decl->getDecl()->getNameAsString();
    if (auto svAttr = decl->findAttribute<SVAttribute>(kSVShaderAttribute))
    {
        newSV.index = 0u;
        newSV.semantic = "SV_" + svAttr->getSemantic();
        std::transform(newSV.semantic.begin(), newSV.semantic.end(), newSV.semantic.begin(), ::toupper);
    }
}

void HLSLStage::recursiveExtractStageSVs(HLSLFlatSVs& hlslSV, const TypeDeclare* declType, const Declare* decl, const AnalysisSystemValues* Ana)
{
    if (auto builtinType = llvm::dyn_cast<BuiltinDeclare>(declType))
    {
        atomicExtractStageSVs(hlslSV, declType, decl, Ana);
    }
    else if (auto structType = llvm::dyn_cast<StructureDeclare>(declType))
    {
        for (auto field : structType->getFields())
        {
            auto fieldType = field->getTypeDeclare();
            if (auto builtin = fieldType ? fieldType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute) : nullptr)
            {
                atomicExtractStageSVs(hlslSV, fieldType, field, Ana);
            }
            else
            {
                recursiveExtractStageSVs(hlslSV, fieldType, field, Ana);
            }
        }
    }
}

void HLSLStage::recursiveExtractStageSVs(HLSLFlatSVs& hlslStage, const AnalysisSystemValues* Ana)
{
    if (auto decl = Ana->getAsDeclare())
    {
        recursiveExtractStageSVs(hlslStage, decl->getTypeDeclare(), decl, Ana);
    }
}

void HLSLStage::extractStageSVs()
{
    // extract
    for (const auto& sv : s.svs)
    {
        recursiveExtractStageSVs(stage_SVs, &sv);
    }

    // remove duplicates
    {
        auto fields = stage_SVs.getFields();
        llvm::SmallVector<HLSLSystemValue> unique;
        unique.reserve(fields.size());
        for (auto& hlslSV : fields)
        {
            auto it = std::find_if(unique.begin(), unique.end(), 
            [&](const HLSLSystemValue& other) {
                return hlslSV.semantic == other.semantic;
            });
            if (it == unique.end())
            {
                unique.emplace_back(hlslSV);
            }
        }
        stage_SVs.fields = std::move(unique);
    }
}

std::string HLSLStage::serializeStageSVs() const
{
    std::string serialized = "";
    auto newline = [&]() { serialized += "\nstatic "; };
    {
        const auto& fields = stage_SVs.getFields();
        if (!fields.empty())
        {
            serialized += "// Stage: ";
            serialized += stage_SVs.HLSLTypeName;
            for (auto sv : fields)
            {
                newline();
                serialized += HLSLTypeToString(sv.type);
                serialized += " ";
                serialized += GetSemanticVarName(stage_SVs.ana->function, sv.semantic.c_str());
                serialized += " : ";
                serialized += sv.semantic;
                serialized += "; // ";
                serialized += getAccessTypeString(sv.ana->getAcessType());
            }
            serialized += "\n";
        }
    }
    return serialized + "\n";
}

std::string HLSLStage::makeStageSVsSignature() const
{
    std::string signature = "";
    auto neslot = [&]() { signature += ",\n    "; };
    {
        const auto& fields = stage_SVs.getFields();
        if (!fields.empty())
        {
            for (auto sv : fields)
            {
                neslot();
                signature += getAccessTypeString(sv.ana->getAcessType());
                signature += " ";
                signature += HLSLTypeToString(sv.type);
                signature += " ";
                signature += GetSemanticVarName(stage_SVs.ana->function, sv.semantic.c_str());
                signature += " : ";
                signature += sv.semantic;
            }
        }
    }
    return signature;
}


}