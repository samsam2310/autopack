#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include "clang/AST/AST.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecordLayout.h"
#include "yaml-cpp/yaml.h"

#include "generator.h"
#include "config.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace std;
using namespace llvm;

static cl::OptionCategory MyToolCategory("wasmtool");

void ParserHandler::setContext(ASTContext &context) {
    this->context = &context;
}

void ParserHandler::run(const MatchFinder::MatchResult &result) {
    if(const FunctionDecl *funcDecl = result.Nodes.getNodeAs<FunctionDecl>("funcDecl")) {
        std::cout<<"function decl!\n";
        funcDecl->dump();
    }
    else if(const CXXRecordDecl *recordDecl = result.Nodes.getNodeAs<CXXRecordDecl>("recordDecl")) {
        std::cout<<"record decl!\n";
        recordDecl->dump();
    }
}

ParserASTConsumer::ParserASTConsumer(CompilerInstance &Instance) : handlerForMatchResult(Instance) {
    matcher.addMatcher(functionDecl().bind("funcDecl"), &handlerForMatchResult);
    matcher.addMatcher(cxxRecordDecl().bind("recordDecl"), &handlerForMatchResult);
}

void ParserASTConsumer::HandleTranslationUnit(ASTContext &context) {
    std::cout<<"translation\n";
    handlerForMatchResult.setContext(context);
    matcher.matchAST(context);
}

void genParseResult(int argc, const char** argv, Config &config) {
    std::vector<std::string> filenames;
    filenames.push_back(config.filename);
    std::cout<<config.filename<<"\n";

    // alloc code to string to load
    ifstream parse_file(config.filename);
    string code_str;
    parse_file.seekg(0, std::ios::end);
    code_str.reserve(parse_file.tellg());
    parse_file.seekg(0, std::ios::beg);
    code_str.assign((std::istreambuf_iterator<char>(parse_file)),
            std::istreambuf_iterator<char>());

    parse_file.close();

    clang::tooling::runToolOnCode(new ParserAction, code_str);
}
