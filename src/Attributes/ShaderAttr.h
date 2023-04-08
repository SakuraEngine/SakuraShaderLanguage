#pragma once
#include "clang/AST/ASTContext.h"
#include <functional>

namespace ssl
{
struct Declare;
using ShaderAttributeKind = uint64_t;
enum : uint64_t
{
    kFirstShaderAttribute = 0u,
    kStageShaderAttribute = 1u,
    kBuiltinShaderAttribute = 2u,
    kSVShaderAttribute = 3u,
    kStageInputAttribute = 4u,
    kAttributeAttribute = 5u,
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
    struct Store
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