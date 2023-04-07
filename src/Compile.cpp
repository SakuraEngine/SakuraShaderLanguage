#include "defines.h"
#include "HLSL/CompileToHLSL.h"

namespace ssl
{
std::string compile_binding(const SourceFile& P)
{
    return "";
}

std::string compile_input(const SourceFile& P)
{
    return "";
}

std::string compile_functions(const SourceFile& P)
{
    return "";
}

std::string compile(const SourceFile& P)
{
    return ssl::hlsl::compile(P);
}
} // namespace ssl