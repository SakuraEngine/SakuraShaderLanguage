#pragma once
#include "clang/AST/ASTContext.h"
#include "./../Source.h"
#include <functional>

namespace ssl
{
enum : uint64_t
{
    kFirstShaderAttribute = 0u,
    kStageShaderAttribute = 1u,
    kBuiltinShaderAttribute = 2u,
    kSVShaderAttribute = 3u,
    kStageInputAttribute = 4u,
    kStageOutputAttribute = 5u,
    kAttributeAttribute = 6u,

    kInternalAttributeStart = 1024u,
    kRegCxxBuiltinAttribute = kInternalAttributeStart,

    kLastShaderAttribute,
};

struct ShaderAttribute
{
public:
    virtual ~ShaderAttribute() = default;

    using CreateFuntion = std::function<ShaderAttribute*(clang::AnnotateAttr*, ssl::Declare* decl)>;
    static bool RegisterType(std::string, CreateFuntion func);
    static ShaderAttribute* Create(clang::AnnotateAttr* annotate, ssl::Declare* decl);

    struct Factory
    {
        static Factory* Get();
        llvm::StringMap<CreateFuntion> createFunctions;
        std::vector<std::unique_ptr<ShaderAttribute>> attributes;
    };

    template <typename T>
    struct Store : public Declare::InternalAccessor, public SourceFile::InternalAccessor
    {
        Store(std::string str, CreateFuntion func)
        {
            auto& createFunctions = ShaderAttribute::Factory::Get()->createFunctions;
            if (createFunctions.find(str) != createFunctions.end())
            {
                return;
            }
            ShaderAttribute::RegisterType(str, func);
        }
    };

    static std::string GetTypeFromAnnotate(clang::AnnotateAttr* attr);
    static std::string GetStringArgFromAnnotate(clang::AnnotateAttr* attr, uint32_t index);
    static int64_t GetIntArgFromAnnotate(clang::AnnotateAttr* attr, uint32_t index);
    std::string GetTypeFromAnnotate() const { return GetTypeFromAnnotate(_this); }
    std::string GetStringArgFromAnnotate(uint32_t index) const { return GetStringArgFromAnnotate(_this, index); }
    int64_t GetIntArgFromAnnotate(uint32_t index) const { return GetIntArgFromAnnotate(_this, index); }

    virtual ShaderAttributeKind getKind() const = 0;

    static bool classof(const ShaderAttribute* A)
    {
        return A->getKind() >= ssl::kFirstShaderAttribute && A->getKind() <= ssl::kLastShaderAttribute;
    }

protected:
    ShaderAttribute(clang::AnnotateAttr* _this);
    std::string type;

    clang::AnnotateAttr* _this;
};

} // namespace ssl