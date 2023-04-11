#pragma once
#include <string>
#include <span>
#include <unordered_map>
#include "clang/AST/Decl.h"

namespace clang { class NamedDecl; }
namespace ssl { class ASTConsumer; struct ShaderAttribute; }

namespace ssl
{
using ShaderAttributeKind = uint64_t;
using TypeDeclareKind = uint64_t;

struct AnalysisStageInput
{
    struct ParameterDeclare* as_param = nullptr;
    struct VarDeclare* as_var = nullptr;
};

struct AnalysisStageOutput
{
    struct ParameterDeclare* as_param = nullptr;
    struct VarDeclare* as_var = nullptr;
    struct RetValDeclare* as_retval = nullptr;
};

struct AnalysisShaderStage
{
    struct FunctionDeclare* function = nullptr;
    llvm::SmallVector<AnalysisStageInput, 1> inputs;
    llvm::SmallVector<AnalysisStageOutput, 1> outputs;
};

struct SourceAnalysis
{
    llvm::SmallVector<AnalysisShaderStage, 2> stages;
};

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
    // std::span<struct SourceFile* const> getIncludes() const { return includes; }
    std::span<struct TypeDeclare* const> getTypes() const { return types; }
    std::span<struct FunctionDeclare* const> getFunctions() const { return functions; }

    struct FunctionDeclare* find(clang::FunctionDecl* decl) const;
    struct TypeDeclare* find(clang::RecordDecl* decl) const;
    const SourceAnalysis& getAnalysis() const { return analysis; }

    struct InternalAccessor
    {
        virtual ~InternalAccessor() = default;
        llvm::SmallVector<struct TypeDeclare*>& getTypeRegistry(SourceFile* src) 
        {
            return src->types; 
        }
    };
    friend struct InternalAccessor;

protected:
    void analyze();

    std::string filename;
    std::string abs_filename;
    // llvm::SmallVector<struct SourceFile*> includes;
    llvm::SmallVector<struct VarDeclare*> vars;
    llvm::SmallVector<struct FunctionDeclare*> functions;
    llvm::SmallVector<struct TypeDeclare*> types;
    SourceAnalysis analysis;
};

struct GlobalDataMap
{
    TypeDeclare* find(clang::RecordDecl* decl) const
    {
        auto file_id = declLocs.find(decl);
        if (file_id == declLocs.end()) return nullptr;
        auto _file = files.find(file_id->second);
        if (_file == files.end()) return nullptr;
        return _file->second->find(decl);
    }

    FunctionDeclare* find(clang::FunctionDecl* decl) const
    {
        auto file_id = declLocs.find(decl);
        if (file_id == declLocs.end()) return nullptr;
        auto _file = files.find(file_id->second);
        if (_file == files.end()) return nullptr;
        return _file->second->find(decl);
    }

    llvm::StringMap<std::unique_ptr<SourceFile>> files;
    std::unordered_map<clang::Decl*, std::string> declLocs;
};

struct CxxBuiltinType
{
    CxxBuiltinType(const clang::BuiltinType* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    ~CxxBuiltinType();
};

struct Declare 
{
    virtual ~Declare();
    Declare() = default;
    Declare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    
    std::string_view getFileId() const { return file_id; }
    clang::NamedDecl* getDecl() const { return decl; }
    clang::Decl::Kind getKind() const { return kind; }

    std::span<struct ShaderAttribute* const> getAttributes() const { return attributes; }
    llvm::SmallVector<ShaderAttribute*> findAttributes(ShaderAttributeKind kind);
    ShaderAttribute* findAttribute(ShaderAttributeKind kind);
    template<typename T>
    T* findAttribute(ShaderAttributeKind kind) 
    { 
        auto raw = findAttribute(kind); 
        return raw ? llvm::dyn_cast<T>(raw) : nullptr; 
    }

    struct InternalAccessor
    {
        virtual ~InternalAccessor() = default;
        GlobalDataMap& getRoot(Declare* decl) { return *decl->root; }
    };
    friend struct InternalAccessor;

protected:
    clang::Decl::Kind kind;
    clang::NamedDecl* decl = nullptr;
    std::string_view file_id;
    llvm::SmallVector<ShaderAttribute*, 16> attributes;
    GlobalDataMap* root = nullptr;
    std::string _debug_name = "uninitialized";
};

struct VarDeclare : public Declare
{
    VarDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
        : Declare(decl, file_id, root)
    {
        
    }
};

struct VarTemplateDeclare : public Declare
{
    VarTemplateDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    ~VarTemplateDeclare();

    VarDeclare* var = nullptr;
};

struct FieldDeclare : public Declare
{
    FieldDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
        : Declare(decl, file_id, root)
    {
        
    }
    TypeDeclare* getTypeDeclare() const;
    // CxxBuiltinType* getCxxBuiltinType() const;
protected:
    // CxxBuiltinType* cxx_builtin = nullptr;
};

enum : uint64_t
{
    kFirstTypeDeclareKind = 0u,
    kTypeDeclare_Builtin = 1u,
    kTypeDeclare_Structure = 2u,
    kLastTypeDeclareKind,
};

struct TypeDeclare : public Declare
{
    friend class ssl::ASTConsumer;

    TypeDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    ~TypeDeclare();

    virtual TypeDeclareKind getKind() const = 0;
    static bool classof(const TypeDeclare* T)
    {
        return T->getKind() >= ssl::kFirstTypeDeclareKind && T->getKind() <= ssl::kLastTypeDeclareKind;
    }
};

struct BuiltinDeclare : public TypeDeclare
{
    friend class ssl::ASTConsumer;

    BuiltinDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    ~BuiltinDeclare();

    TypeDeclareKind getKind() const override { return kTypeDeclare_Builtin; }
    static bool classof(const TypeDeclare* A)
    {
        return A->getKind() == ssl::kTypeDeclare_Builtin;
    }
};

struct StructureDeclare : public TypeDeclare
{
    friend class ssl::ASTConsumer;

    StructureDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    ~StructureDeclare();

    std::span<struct FieldDeclare* const> getFields() const { return fields; }

    TypeDeclareKind getKind() const override { return kTypeDeclare_Structure; }
    static bool classof(const TypeDeclare* A)
    {
        return A->getKind() == ssl::kTypeDeclare_Structure;
    }
protected:
    llvm::SmallVector<FieldDeclare*, 16> fields;
};

struct ParameterDeclare : public Declare
{
    ParameterDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
        : Declare(decl, file_id, root)
    {
        
    }
    TypeDeclare* getTypeDeclare() const;
};

struct FunctionDeclare : public Declare
{
    friend class ssl::ASTConsumer;

    FunctionDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    ~FunctionDeclare();

    bool is(clang::FunctionDecl* decl) const { return this->decl == decl; }
    
    std::span<struct VarDeclare* const> getVars() const { return vars; }
    std::span<struct ParameterDeclare* const> getParameters() const { return parameters; }
protected:
    llvm::SmallVector<VarDeclare*, 16> vars;
    llvm::SmallVector<ParameterDeclare*, 8> parameters;
};

std::string compile(const SourceFile& P);
} // namespace ssl