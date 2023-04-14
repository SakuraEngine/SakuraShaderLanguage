#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

HLSLParameter::HLSLParameter(const HLSLShaderLibrary* root, const TypeDeclare* declType, const ParameterDeclare* decl, struct HLSLFunction* funcType)
    : declType(declType), decl(decl), funcType(funcType) 
{
    type = root->FindHLSLType(declType);
    name = decl->getDecl()->getNameAsString();
    if (auto svattr = decl->findAttribute<SVAttribute>(kSVShaderAttribute))
    {
        semantic = svattr->getSemantic();
    }
}

bool HLSLParameter::isSystemValue() const
{ 
    return decl->findAttribute(kSVShaderAttribute);
}

HLSLFunction::HLSLFunction(const HLSLShaderLibrary* root,const FunctionDeclare* decl)
    : declare(decl)
{
    
}

void HLSLShaderLibrary::makeFunctions()
{
    functions.resize(f.getFunctions().size());
    uint32_t funcIndex = 0;
    for (const auto& func : f.getFunctions())
    {
        auto& function = functions[funcIndex];
        function = HLSLFunction(this, func);
        for (const auto& s : f.getAnalysis().stages)
        {
            if (s.function == func) stages.emplace_back(this, s, function);
        }

        if (auto fdecl = llvm::dyn_cast<FunctionDeclare>(func))
        {
            for (auto param : fdecl->getParameters())
            {
                auto paramType = param->getTypeDeclare();
                if (!paramType) continue;
                function.parameters.emplace_back(this, paramType, param, &function);
            }
        }

        funcIndex++;
    }
}

std::string HLSLShaderLibrary::serializeFunctions() const
{
    std::string serialized = "";

    return serialized + "\n";
}

}