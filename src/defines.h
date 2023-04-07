#pragma once
#include <string>
#include <span>
#include <unordered_map>
#include "clang/AST/Decl.h"

namespace clang { class NamedDecl; }
namespace ssl { class ASTConsumer; struct ShaderAttribute; }

namespace ssl
{
struct SourceFile 
{
    friend class ssl::ASTConsumer;
    SourceFile() = default;
    SourceFile(std::string filename, std::string abs_filename)
        : filename(filename), abs_filename(abs_filename)
    {
    }
    ~SourceFile();

    std::string_view getFilename() const { return filename; }
    std::string_view getAbsFilename() const { return abs_filename; }
    std::span<struct StructDeclare* const> getStructs() const { return structs; }
    std::span<struct FunctionDeclare* const> getFunctions() const { return functions; }

protected:
    std::string filename;
    std::string abs_filename;
    std::vector<struct FunctionDeclare*> functions;
    std::vector<struct StructDeclare*> structs;
};

struct Declare 
{
    Declare() = default;
    Declare(clang::NamedDecl* decl, std::string_view file_id);

    std::string_view getFileId() const { return file_id; }
    clang::NamedDecl* getDecl() const { return decl; }
    clang::Decl::Kind getKind() const { return kind; }
    std::span<struct ShaderAttribute* const> getAttributes() const { return attributes; }

protected:
    clang::Decl::Kind kind;
    clang::NamedDecl* decl = nullptr;
    std::string_view file_id;
    llvm::SmallVector<ShaderAttribute*, 16> attributes;
    // SourceFile* _file = nullptr;
};

struct FieldDeclare : public Declare
{
    FieldDeclare(clang::NamedDecl* decl, std::string_view file_id)
        : Declare(decl, file_id)
    {
        
    }
};

struct StructDeclare : public Declare
{
    StructDeclare(clang::NamedDecl* decl, std::string_view file_id);
    ~StructDeclare();
    llvm::SmallVector<FieldDeclare*, 16> fields;
};

struct ParameterDeclare : public Declare
{
    ParameterDeclare(clang::NamedDecl* decl, std::string_view file_id)
        : Declare(decl, file_id)
    {
        
    }
};

struct FunctionDeclare : public Declare
{
    FunctionDeclare(clang::NamedDecl* decl, std::string_view file_id);
    ~FunctionDeclare();
    
    llvm::SmallVector<ParameterDeclare*, 8> parameters;
};

using FileDataMap = std::unordered_map<std::string, SourceFile>;

std::string compile(const SourceFile& P);
} // namespace ssl