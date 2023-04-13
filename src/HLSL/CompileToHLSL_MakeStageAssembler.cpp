#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

void HLSLShaderLibrary::makeAssemblers()
{
    const auto& source = f;
    const auto& ana = f.getAnalysis();

}

std::string HLSLShaderLibrary::serializeAssemblers() const
{
    std::string serialized = "";
    serialized += "// 6. HLSL stage input/output assemblers\n";

    return serialized;
}

}