#include "./../Source.h"
#include "./../Attributes/ShaderAttr.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include <iostream>

namespace ssl
{

TypeDeclare::TypeDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : TypedDeclare(decl, file_id, root)
{

}

TypeDeclare::~TypeDeclare()
{

}

BuiltinDeclare::BuiltinDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : TypeDeclare(decl, file_id, root)
{

}

BuiltinDeclare::~BuiltinDeclare()
{

}

StructureDeclare::StructureDeclare(clang::NamedDecl* decl, std::string_view file_id, ssl::GlobalDataMap* root)
    : TypeDeclare(decl, file_id, root)
{
    if (auto record = llvm::dyn_cast<clang::RecordDecl>(decl))
    {
        for (auto field : record->fields())
        {
            fields.emplace_back(new FieldDeclare(field, file_id, root));
        }
    }
}

StructureDeclare::~StructureDeclare()
{
    for (auto field : fields) delete field;
}

}