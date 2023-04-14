#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

void HLSLStage::atomicExtractStageSVs(HLSLFlatSVs& hlslSV, const TypeDeclare* declType, const Declare* decl, const AnalysisSystemValues* Ana)
{
    HLSLSystemValue& newSV = hlslSV.fields.emplace_back();
    newSV.ana = Ana;
    if (auto builtin = declType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute))
    {
        newSV.type = StringToHLSLType(builtin->getBuiltinName().c_str());
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
    auto& hlslSV = stage_SVs.emplace_back(&s);
    for (const auto& sv : s.svs)
    {
        recursiveExtractStageSVs(hlslSV, &sv);
    }

    // remove duplicates
    for (auto& hlslSVs : stage_SVs)
    {
        auto fields = hlslSVs.getFields();
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
        hlslSVs.fields = std::move(unique);
    }
}

std::string HLSLStage::serializeStageSVs() const
{
    std::string serialized = "";
    auto newline = [&]() { serialized += "\nstatic "; };
    for (const auto& stageSV : stage_SVs)
    {
        const auto& fields = stageSV.getFields();
        if (!fields.empty())
        {
            serialized += "// Stage: ";
            serialized += stageSV.HLSLTypeName;
            for (auto sv : fields)
            {
                newline();
                serialized += HLSLTypeToString(sv.type);
                serialized += " ";
                serialized += GetSemanticVarName(stageSV.ana->function, sv.semantic.c_str());
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

}