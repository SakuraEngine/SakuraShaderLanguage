#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

void HLSLShaderLibrary::makeFunctions()
{
    const auto& source = f;
    const auto& ana = f.getAnalysis();

}

std::string HLSLShaderLibrary::serializeFunctions() const
{
    std::string serialized = "";

    return serialized;
}

}