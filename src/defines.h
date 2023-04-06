#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace ssl
{
struct SourceFile {
    std::string attrs;
};

using FileDataMap = std::unordered_map<std::string, SourceFile>;

std::string compile(const SourceFile& P);
} // namespace ssl