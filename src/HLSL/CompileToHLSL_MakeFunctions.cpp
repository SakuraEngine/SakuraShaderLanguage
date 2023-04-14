#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

void HLSLStage::makeFunctions()
{

}

std::string HLSLStage::serializeFunctions() const
{
    std::string serialized = "";

    return serialized + "\n";
}

}