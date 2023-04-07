#include "ShaderAttr.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace ssl
{
ShaderAttribute::Factory* ShaderAttribute::Factory::Get()
{
    static Factory factory;
    return &factory;
}

ShaderAttribute::ShaderAttribute(clang::AnnotateAttr* _this)
    : _this(_this)
{

}

bool ShaderAttribute::RegisterType(std::string type, CreateFuntion function)
{
    auto& createFunctions = ShaderAttribute::Factory::Get()->createFunctions;
    if (createFunctions.find(type) != createFunctions.end())
    {
        return false;
    }
    createFunctions[type] = function;
    return true;
}

ShaderAttribute* ShaderAttribute::Create(clang::AnnotateAttr* annotate, ssl::Declare* decl)
{
    auto& createFunctions = ShaderAttribute::Factory::Get()->createFunctions;
    auto expr = *annotate->args_begin();
    if (auto TypeLiterial = llvm::dyn_cast<clang::StringLiteral>(expr->IgnoreParenCasts()))
    {
        auto type = std::string(TypeLiterial->getString());
        if (createFunctions.find(type) == createFunctions.end())
        {
            return nullptr;
        }
        return createFunctions[type](annotate, decl);
    }
    return nullptr;
}

std::string ShaderAttribute::GetTypeFromAnnotate(clang::AnnotateAttr* annotate)
{
    auto iter = annotate->args_begin();
    if (auto TypeLiterial = llvm::dyn_cast<clang::StringLiteral>((*iter)->IgnoreParenCasts()))
    {
        auto type = std::string(TypeLiterial->getString());
        return type;
    }
    return "";
}

std::string ShaderAttribute::GetStringArgFromAnnotate(clang::AnnotateAttr* annotate, uint32_t index)
{
    auto iter = annotate->args_begin();
    iter++;
    iter += index;
    if (auto TypeLiterial = llvm::dyn_cast<clang::StringLiteral>((*iter)->IgnoreParenCasts()))
    {
        auto type = std::string(TypeLiterial->getString());
        return type;
    }
    return "";
}

int64_t ShaderAttribute::GetIntArgFromAnnotate(clang::AnnotateAttr* annotate, uint32_t index)
{
    auto iter = annotate->args_begin();
    iter++; 
    iter += index;
    if (auto IntLiterial = llvm::dyn_cast<clang::IntegerLiteral>((*iter)->IgnoreParenCasts()))
    {
        return IntLiterial->getValue().getLimitedValue();
    }
    return 0;
}

}