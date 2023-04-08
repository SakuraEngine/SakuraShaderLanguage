// Declares clang::SyntaxOnlyAction.
#include "clang/AST/Decl.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

/*
#include "llvm/Support/Regex.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Core/TokenRewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "clang/Tooling/Core/Replacement.h"
*/

// Declares llvm::cl::extrahelp.
#include "ASTConsumer.h"
#include "OptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Path.h"
#include <iostream>
#include <memory>

namespace tooling = clang::tooling;
// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory ToolCategoryOption("meta options");
static llvm::cl::cat ToolCategory(ToolCategoryOption);

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static llvm::cl::extrahelp
CommonHelp(tooling::CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static llvm::cl::extrahelp MoreHelp("\nMore help text...\n");

static llvm::cl::opt<std::string> Output(
    "output", llvm::cl::Optional,
    llvm::cl::desc("Specify database output directory, depending on extension"),
    ToolCategory, llvm::cl::value_desc("directory"));

class SSLFrontendAction : public clang::ASTFrontendAction
{
public:
    SSLFrontendAction() {}

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& CI, llvm::StringRef InFile)
    {
        auto OutputPath = Output.hasArgStr() ? Output.getValue() : "./";
        auto& PP = CI.getPreprocessor();
        clang::SourceManager& SM = PP.getSourceManager();
        auto& LO = CI.getLangOpts();
        LO.CommentOpts.ParseAllComments = true;
        return std::make_unique<ssl::ASTConsumer>(OutputPath, datamap);
    }
    
    ssl::GlobalDataMap datamap;
};

int main(int argc, const char** argv)
{
    std::vector<const char*> args(argc + 1);
    args[0] = argv[0];
    for (int i = 1; i < argc; ++i)
        args[i + 1] = argv[i];
    args[1] = "--extra-arg=-D__SSL__";
    argc += 1;
    auto ExpectedParser = ssl::OptionsParser::create(
    argc, args.data(), llvm::cl::ZeroOrMore, ToolCategoryOption);
    if (!ExpectedParser)
    {
        // Fail gracefully for unsupported options.
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    ssl::OptionsParser& OptionsParser = ExpectedParser.get();
    tooling::ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
    return Tool.run(tooling::newFrontendActionFactory<SSLFrontendAction>().get());
}