#include "Source.h"
#include "HLSL/CompileToHLSL.h"

namespace ssl
{

std::string compile(const SourceFile& P)
{
    return ssl::hlsl::compile(P);
}
} // namespace ssl