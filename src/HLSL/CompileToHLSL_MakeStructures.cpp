#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

void HLSLShaderLibrary::makeStructures()
{
    const auto& source = f;
    const auto& ana = f.getAnalysis();

    for (auto type : source.getTypes())
    {
        if (auto structure = llvm::dyn_cast<StructureDeclare>(type))
        {
            auto& s = structures.emplace_back(structure);
            for (auto field : structure->getFields())
            {
                auto fieldType = field->getTypeDeclare();
                if (!fieldType) continue;
        
                if (auto builtin = fieldType ? fieldType->findAttribute<BuiltinAttribute>(kBuiltinShaderAttribute) : nullptr)
                {
                    s.fields.emplace_back(fieldType, field, nullptr);
                }
                else
                {
                    s.fields.emplace_back(fieldType, field, nullptr);
                }
            }
            if (!s.fields.size()) structures.pop_back();
        }
    }
}

std::string HLSLShaderLibrary::serializeStructures() const
{
    std::string serialized = "";
    // comment structures
    serialized += "// used structures: ";
    uint32_t c = 0;
    for (const auto& structure : structures)
    {
        if (c++ > 8) serialized += "\n//";
        serialized += structure.getHLSLTypeName();
        serialized += ", ";
    }
    serialized += "\n";
    
    // serialize structures
    for (const auto& structure : structures)
    {
        // output validate
        uint32_t filled_count = 0;
        for (const auto& field : structure.getFields())
        {
            if (field.type == HLSLType::Unknown || field.type == HLSLType::Void) continue;
            filled_count++;
        }
        if (!filled_count) continue; // skip empty structures

        // do serialize
        serialized += "struct " + std::string(structure.getHLSLTypeName()) + " {";
        for (const auto& field : structure.getFields())
        {
            serialized += "\n    ";
            serialized += HLSLTypeToString(field.type);
            serialized += " ";
            serialized += field.name;
            serialized += ";";
        }
        serialized += "\n};\n\n";
    }
    return serialized;
}

}