#include "RegCxxBuiltinAttr.h"
#include "./../../Source.h"

namespace ssl
{
RegCxxBuiltinAttribute::RegCxxBuiltinAttribute(clang::AnnotateAttr* _this, ssl::Declare* declare)
    : ShaderAttribute(_this), declare(declare)
{

}

static ShaderAttribute::Store<RegCxxBuiltinAttribute> CxxRegStore("_reg_cxx_builtin", 
    [](clang::AnnotateAttr* attr, ssl::Declare* decl) -> ShaderAttribute*
    { 
        if (decl->getKind() != clang::Decl::CXXRecord &&
            decl->getKind() != clang::Decl::Record)
        {
            return nullptr;
        }
        // fetch and register primitives here
        auto clangDecl = decl->getDecl();
        if (auto r = llvm::dyn_cast<clang::RecordDecl>(clangDecl))
        {
        for(auto f = r->field_begin(); f != r->field_end();++f)
        {
        if (auto bt = f->getType()->getAs<clang::BuiltinType>())
        {
            const auto fid = decl->getFileId();
            auto& datamap = CxxRegStore.getRoot(decl);
            auto& db = datamap.files[fid.data()];
            // CxxRegStore.getTypeRegistry(db.get()).emplace_back(
            //    new CxxBuiltinType(bt, std::string_view(fid), &datamap));
        }
        }
        }
        return new RegCxxBuiltinAttribute(attr, decl); 
    }
);

}