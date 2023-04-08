#pragma once
#include "Source.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/RecordLayout.h"
#include <unordered_set>

#define LOG(...)
#define LOG_PUSH_IDENT
#define LOG_POP_IDENT

namespace ssl
{
using namespace clang;

enum ParseBehavior
{
    PAR_Normal,
    PAR_Reflect,
    PAR_NoReflect
};

class ASTConsumer : public clang::ASTConsumer
{
public:
    ASTConsumer(std::string OutputPath, GlobalDataMap& datamap)
        : OutputPath(OutputPath), datamap(datamap)
    {
    }
    void HandleTranslationUnit(ASTContext& ctx) override;
    ASTContext* GetContext() { return _ASTContext; }
    const GlobalDataMap& GetDataMap() { return datamap; }
    
protected:
    void HandleDecl(clang::NamedDecl* decl, std::vector<std::string>& attrStack, ParseBehavior behavior, const clang::ASTRecordLayout* layout);
    void HandleRecord(clang::NamedDecl* decl, std::vector<std::string>& attrStack, ParseBehavior behavior, const clang::ASTRecordLayout* layout);

    std::string OutputPath;
    GlobalDataMap& datamap;
    ASTContext* _ASTContext;
};
} // namespace ssl