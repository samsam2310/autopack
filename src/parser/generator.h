#include "clang/AST/AST.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecordLayout.h"

#include "config.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace std;
using namespace llvm;

#ifndef GENERATOR_H
#define GENERATOR_H

class ParserHandler : public MatchFinder::MatchCallback {
private:
    CompilerInstance &Instance;
    ASTContext *context;

public:
    ParserHandler(CompilerInstance &Instance) : Instance(Instance) {}
    void setContext(ASTContext &context);
    // implement node handler here
    virtual void run(const MatchFinder::MatchResult &result);
};

class ParserASTConsumer : public clang::ASTConsumer {
public:
    // define match node type in this function
    // use matcher.addMatcher()
    ParserASTConsumer(CompilerInstance &Instance);

private:
    MatchFinder matcher;
    ParserHandler handlerForMatchResult;

    void HandleTranslationUnit(ASTContext &context);
};

class ParserAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            CompilerInstance &Instance, StringRef InFile) {
        return std::unique_ptr<ParserASTConsumer>(
            new ParserASTConsumer(Instance)
        );
    }
};


void genParseResult(int argc, const char** argv, Config &config);

#endif
