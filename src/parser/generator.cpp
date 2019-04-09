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
#include "utils.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace std;
using namespace llvm;


class ParserHandler : public MatchFinder::MatchCallback {
private:
    CompilerInstance &Instance;
    ASTContext *context;

public:
    ParserHandler(CompilerInstance &Instance, Generator &generator) :
        Instance(Instance), _generator(generator) {}
    void setContext(ASTContext &context);
    // implement node handler here
    virtual void run(const MatchFinder::MatchResult &result);
private:
    void handleFunctionDecl(const FunctionDecl *funcDecl);
    void handleCXXRecordDecl(const CXXRecordDecl *recordDecl);
    Generator &_generator;
};

class ParserASTConsumer : public clang::ASTConsumer {
public:
    // define match node type in this function
    // use matcher.addMatcher()
    ParserASTConsumer(CompilerInstance &Instance, Generator &generator);

private:
    MatchFinder matcher;
    ParserHandler handlerForMatchResult;

    void HandleTranslationUnit(ASTContext &context);
};

class ParserAction : public clang::ASTFrontendAction {
public:
    ParserAction(Generator &generator) : _generator(generator) {}
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            CompilerInstance &Instance, StringRef InFile) {
        ParserASTConsumer *consumer = new ParserASTConsumer(Instance, _generator);
        return std::unique_ptr<ParserASTConsumer>(consumer);
    }

private:
    Generator &_generator;
};

void ParserHandler::setContext(ASTContext &context) {
    this->context = &context;
}

void ParserHandler::run(const MatchFinder::MatchResult &result) {
    if(const FunctionDecl *funcDecl = result.Nodes.getNodeAs<FunctionDecl>("funcDecl")) {
        handleFunctionDecl(funcDecl);
    }
    else if(const CXXRecordDecl *recordDecl = result.Nodes.getNodeAs<CXXRecordDecl>("recordDecl")) {
        std::cout<<"record decl!\n";
        recordDecl->dump();
    }
}

void ParserHandler::handleFunctionDecl(const FunctionDecl* funcDecl) {
    std::cerr<<"==func decl==\n";
    if(!(
        funcDecl->isFirstDecl() &&
        !funcDecl->isMain() && 
        !funcDecl->isCXXClassMember())) {
        std::cerr<<"==skipped==\n";
        return;
    }
    funcDecl->dump();

    ExportUnit unit;
    unit.js_data.name = funcDecl->getNameInfo().getName().getAsString();
    
    
    _generator.all_unit.push_back(unit);
}

ParserASTConsumer::ParserASTConsumer(CompilerInstance &Instance, Generator &generator) :
        handlerForMatchResult(Instance, generator) {
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

    Generator gen;
    ParserAction *action = new ParserAction(gen);

    clang::tooling::runToolOnCode(action, code_str);
}
