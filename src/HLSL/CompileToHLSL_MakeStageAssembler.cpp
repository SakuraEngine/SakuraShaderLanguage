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
    {
        auto stageAttr = llvm::dyn_cast<StageAttribute>(stage_SVs.ana->stage_attr);
        // comment
        serialized += "// Assembler for ";
        serialized += stageAttr->getStageName();
        serialized += " stage (";
        serialized += stage_SVs.ana->function->getDecl()->getQualifiedNameAsString();
        serialized += ")\n";

        // signature
        serialized += stage_output.getHLSLTypeName();
        serialized += " ";
        serialized += s.function->getDecl()->getQualifiedNameAsString();
        serialized += "(";
        serialized += stage_input.getHLSLTypeName();
        serialized += " input";
        serialized += makeStageSVsSignature();
        serialized += ")\n";

        // body
        serialized += "{";
        auto newline = [&]() { serialized += "\n    "; };

        // declare output
        newline();
        serialized += stage_output.getHLSLTypeName();
        serialized += " outputs = (";
        serialized += stage_output.getHLSLTypeName();
        serialized += ")0;";

        // assemble inputs
        for (const auto& param : func.parameters)
        {
            newline();
            // declare on stack
            serialized += HLSLTypeToString(param.getType());
            serialized += " ";
            serialized += param.getName();
            serialized += " = (";
            serialized += HLSLTypeToString(param.getType());
            serialized += ")0;";
        
            // fill with semantic
            auto pType = param.getType();
            if (pType != HLSLType::Struct)
            {
                newline();
                serialized += param.getName();
                // sv
                if (param.isSystemValue())
                {
                    serialized += " = ";
                    serialized += GetSemanticVarName(this->func.declare, param.getSemantic());
                    serialized += ";";
                }
                // normal semantic
                else
                {
                    serialized += " = input.";
                    serialized += param.getName();
                    serialized += ";";
                }
            }
            else
            {
                auto plainStruct = (HLSLPlainStruct*)pType.structType;
                for (const auto& field : plainStruct->fields)
                {
                    newline();
                    serialized += param.getName();
                    serialized += ".";
                    serialized += field.getName();
                    // sv
                    if (param.isSystemValue())
                    {
                        serialized += " = ";
                        serialized += GetSemanticVarName(this->func.declare, param.getSemantic());
                        serialized += ";";
                    }
                    // normal semantic
                    else
                    {
                        serialized += " = input.";
                        serialized += field.getName();
                        serialized += ";";
                    }
                }
            }
        }

        // call generated function


        // return outputs
        if (!stage_output.getFields().empty())
        {
            newline();
            serialized += "return outputs;\n";
        }
        serialized += "\n}\n";
    }
    return serialized + "\n";
}

}