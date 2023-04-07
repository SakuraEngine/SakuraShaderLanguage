#include "CompileToHLSL.h"
#include "../Attributes/BuiltinAttr.h"
#include "../Attributes/StageAttr.h"

namespace ssl::hlsl
{

HLSLShaderLibrary::HLSLShaderLibrary(const SourceFile& f, const HLSLOptions& options)
    : f(f)
{
    for (auto func : f.getFunctions())
    {
        for (auto attr : func->getAttributes())
        {
            if (auto builtin = llvm::dyn_cast<BuiltinAttribute>(attr))
            {
                builtins.emplace_back(func);
            }
            if (auto stage = llvm::dyn_cast<StageAttribute>(attr))
            {
                stages.emplace_back(func);
            }
        }
    }
}

void HLSLShaderLibrary::translate()
{

}

std::string HLSLShaderLibrary::serialize() const
{
    return "";
}

std::string compile(const SourceFile& P, const HLSLOptions& options)
{
    HLSLShaderLibrary lib(P, options);
    lib.translate();
    return lib.serialize();
}
}