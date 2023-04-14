#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"
#include "../Attributes/AttrAttr.h"
#include "../Attributes/SvAttr.h"

namespace ssl::hlsl
{

void HLSLStage::makeAssemblers()
{

}

std::string HLSLStage::serializeAssemblers() const
{
    std::string serialized = "";
    for (const auto& stageSV : stage_SVs)
    {
        auto stageAttr = llvm::dyn_cast<StageAttribute>(stageSV.ana->stage_attr);
        // comment
        serialized += "// Assembler for ";
        serialized += stageAttr->getStageName();
        serialized += " stage (";
        serialized += stageSV.ana->function->getDecl()->getQualifiedNameAsString();
        serialized += ")\n";

        // signature
        serialized += "";
    }
    return serialized + "\n";
}

}