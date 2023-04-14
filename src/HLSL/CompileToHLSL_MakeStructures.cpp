#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

HLSLField::HLSLField(const HLSLShaderLibrary* root, const TypeDeclare* declType, const FieldDeclare* decl)
    : HLSLElement(root), declType(declType), decl(decl)
{
    type = root->FindHLSLType(declType);
    name = decl->getDecl()->getNameAsString();
}

HLSLPlainStruct::HLSLPlainStruct(const HLSLShaderLibrary* root, const StructureDeclare* decl)
    : HLSLStruct(root), decl(decl)
{
    auto fullName = decl->getDecl()->getQualifiedNameAsString();
    // replace :: to __
    std::replace(fullName.begin(), fullName.end(), ':', '_');
    HLSLTypeName = fullName;
}

void HLSLShaderLibrary::makeStructures()
{
    const auto& source = f;
    const auto& ana = f.getAnalysis();

    for (auto type : source.getTypes())
    {
        if (auto structure = llvm::dyn_cast<StructureDeclare>(type))
        {
            auto& s = structures.emplace_back(
                new HLSLPlainStruct(this, structure)
            );
            for (auto field : structure->getFields())
            {
                auto fieldType = field->getTypeDeclare();
                if (!fieldType) continue;
                s->fields.emplace_back(this, fieldType, field);
            }
            if (!s->fields.size()) structures.pop_back();
        }
    }
}

std::string HLSLShaderLibrary::serializeStructures() const
{
    std::string serialized = "";
    // comment structures
    serialized += "//    used structures: ";
    uint32_t c = 0;
    for (const auto& structure : structures)
    {
        if (c++ > 8) serialized += "\n//";
        serialized += structure->getHLSLTypeName();
        serialized += ", ";
    }
    serialized += "\n";
    
    // serialize structures
    for (const auto& structure : structures)
    {
        // output validate
        uint32_t filled_count = 0;
        for (const auto& field : structure->getFields())
        {
            if (field.type == HLSLType::Unknown || field.type == HLSLType::Void) continue;
            filled_count++;
        }
        if (!filled_count) continue; // skip empty structures

        // do serialize
        serialized += "struct " + std::string(structure->getHLSLTypeName()) + " {";
        for (const auto& field : structure->getFields())
        {
            serialized += "\n    ";
            serialized += HLSLTypeToString(field.getType());
            serialized += " ";
            serialized += field.getName();
            serialized += ";";
        }
        serialized += "\n};\n\n";
    }
    return serialized;
}

}