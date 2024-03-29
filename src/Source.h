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

enum AccessTypeFlags
{
    kAccessNone = 0x00,
    kAccessReading = 0x0000001,
    kAccessWriting = 0x0000002,
    kAccessReadWrite = kAccessReading | kAccessWriting,
    kAcessTypeInvalid = UINT32_MAX
};
using AccessType = uint32_t;

const char* getAccessTypeString(AccessType acess);

struct AnalysisStageInput
{
    AnalysisStageInput() = default;
    AnalysisStageInput(const struct ParameterDeclare* param) : as_param(param) { }
    AnalysisStageInput(const struct VarDeclare* var) : as_var(var) { }

    const struct TypedDeclare* getAsDeclare() const
    {
        if (as_param) return (const struct TypedDeclare*)as_param;
        if (as_var) return (const struct TypedDeclare*)as_var;
        return nullptr;
    }
    const struct ParameterDeclare* getAsParam() const { return as_param; }
    const struct VarDeclare* getAsVar() const { return as_var; }
    
protected:
    const struct ParameterDeclare* as_param = nullptr;
    const struct VarDeclare* as_var = nullptr;
};

struct AnalysisStageOutput
{
    AnalysisStageOutput() = default;
    AnalysisStageOutput(const struct ParameterDeclare* param) : as_param(param) { }
    AnalysisStageOutput(const struct VarDeclare* var) : as_var(var) { }
    AnalysisStageOutput(const struct TypeDeclare* retval) : as_retval(retval) { }

    const struct TypedDeclare* getAsDeclare() const
    {
        if (as_param) return (const struct TypedDeclare*)as_param;
        if (as_var) return (const struct TypedDeclare*)as_var;
        if (as_retval) return (const struct TypedDeclare*)as_retval;
        return nullptr;
    }
    const struct ParameterDeclare* getAsParam() const { return as_param; }
    const struct VarDeclare* getAsVar() const { return as_var; }
    const struct TypeDeclare* getAsRetVal() const { return as_retval; }

protected:
    const struct ParameterDeclare* as_param = nullptr;
    const struct VarDeclare* as_var = nullptr;
    const struct TypeDeclare* as_retval = nullptr;
};

struct AnalysisSystemValues
{
    AnalysisSystemValues() = default;
    AnalysisSystemValues(const struct ParameterDeclare* param, AccessType acess)
        : as_param(param), acess(acess)
    {
    }
    AnalysisSystemValues(const struct VarDeclare* var, AccessType acess)
        : as_var(var), acess(acess)
    {
    }
    AnalysisSystemValues(const struct TypeDeclare* retval)
        : as_retval(retval), acess(kAccessWriting)
    {
    }

    const struct TypedDeclare* getAsDeclare() const
    {
        if (as_param) return (const struct TypedDeclare*)as_param;
        if (as_var) return (const struct TypedDeclare*)as_var;
        if (as_retval) return (const struct TypedDeclare*)as_retval;
        return nullptr;
    }
    const struct ParameterDeclare* getAsParam() const { return as_param; }
    const struct VarDeclare* getAsVar() const { return as_var; }
    const struct TypeDeclare* getAsRetVal() const { return as_retval; }
    AccessType getAcessType() const { return acess; }
protected:
    const struct ParameterDeclare* as_param = nullptr;
    const struct VarDeclare* as_var = nullptr;
    const struct TypeDeclare* as_retval = nullptr;
    AccessType acess = kAcessTypeInvalid;
};

struct AnalysisShaderStage
{
    struct FunctionDeclare* function = nullptr;
    ShaderAttribute* stage_attr = nullptr;
    llvm::SmallVector<AnalysisStageInput, 1> inputs;
    llvm::SmallVector<AnalysisStageOutput, 1> outputs;
    llvm::SmallVector<AnalysisSystemValues> svs;
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
    struct VarDeclare* find(clang::ValueDecl* decl) const;

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

    VarDeclare* find(clang::ValueDecl* decl) const
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
    const llvm::SmallVector<ShaderAttribute*> findAttributes(ShaderAttributeKind kind) const;
    const ShaderAttribute* findAttribute(ShaderAttributeKind kind) const;
    template<typename T>
    const T* findAttribute(ShaderAttributeKind kind) const
    { 
        auto raw = findAttribute(kind); 
        return raw ? llvm::dyn_cast<T>(raw) : nullptr; 
    }

    virtual void analyze(SourceFile* src) {}

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

enum : uint64_t
{
    kFirstTypeDeclareKind = 0u,
    kTypeDeclare_Builtin = 1u,
    kTypeDeclare_Structure = 2u,
    kLastTypeDeclareKind,
};

struct TypedDeclare : public Declare
{
    TypedDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    virtual const TypeDeclare* getTypeDeclare() const = 0;
};

struct TypeDeclare : public TypedDeclare
{
    friend class ssl::ASTConsumer;

    TypeDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    ~TypeDeclare();

    virtual TypeDeclareKind getKind() const = 0;
    static bool classof(const TypeDeclare* T)
    {
        return T->getKind() >= ssl::kFirstTypeDeclareKind && T->getKind() <= ssl::kLastTypeDeclareKind;
    }
    
    const TypeDeclare* getTypeDeclare() const override
    {
        return this;
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

struct VarDeclare : public TypedDeclare
{
    VarDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    virtual ~VarDeclare();
    const TypeDeclare* getTypeDeclare() const;

    bool isStageInput() const;
    bool isStageOutput() const;
protected:
    void queryStageInOut(bool* in, bool* out) const;
};

struct GlobalVarDeclare : public VarDeclare
{
    GlobalVarDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
};

struct LocalVarDeclare : public VarDeclare
{
    LocalVarDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
};

struct VarTemplateDeclare : public Declare
{
    VarTemplateDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    ~VarTemplateDeclare();

    VarDeclare* var = nullptr;
};

struct FieldDeclare : public TypedDeclare
{
    FieldDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
        : TypedDeclare(decl, file_id, root)
    {
        
    }
    const TypeDeclare* getTypeDeclare() const;
};

struct ParameterDeclare : public VarDeclare
{
    ParameterDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    const TypeDeclare* getTypeDeclare() const;
};

struct FunctionDeclare : public Declare
{
    friend class LocalVarRecorder;
    friend class VarRecorder;
    friend class ssl::ASTConsumer;

    FunctionDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root);
    ~FunctionDeclare();

    bool is(clang::FunctionDecl* decl) const { return this->decl == decl; }
    
    std::span<struct LocalVarDeclare* const> getLocalVars() const { return local_vars; }
    std::span<struct VarDeclare* const> getOutterVars() const { return outter_vars; }
    std::span<struct ParameterDeclare* const> getParameters() const { return parameters; }

    const TypeDeclare* getReturnType() const;

    AccessType getAccessType(ParameterDeclare* param) const;
    AccessType getAccessType(VarDeclare* param) const;

    virtual void analyze(SourceFile* src) override;

protected:
    llvm::SmallVector<LocalVarDeclare*, 16> local_vars;
    llvm::SmallVector<VarDeclare*, 16> outter_vars;
    llvm::SmallVector<ParameterDeclare*, 8> parameters;
    // llvm::SmallVector<LocalVarDeclare*, 8> param_vars;
};

std::string compile(const SourceFile& P);
} // namespace ssl