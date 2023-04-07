#pragma once
#include "./../defines.h"

namespace ssl
{
namespace hlsl
{

struct HLSLStageInput
{
    
};

struct HLSLOptions
{
    bool CommentSourceLine = false;
};

struct HLSLShaderLibrary
{
    HLSLShaderLibrary(const SourceFile& f, const HLSLOptions& options);

    std::string serialize() const;
    void translate();

    const SourceFile& f;
    llvm::SmallVector<FunctionDeclare*> stages;
    llvm::SmallVector<FunctionDeclare*> builtins;
};

std::string compile(const SourceFile& P, const HLSLOptions& options = {});
}
} // namespace ssl