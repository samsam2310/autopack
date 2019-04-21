#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include "boost/format.hpp"
#include "clang/AST/AST.h"
#include "clang/AST/Type.h"
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
    void handle_argument(ArgumentData &data, QualType arg);
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

void ParserHandler::handle_argument(ArgumentData &data, QualType arg) {
    auto orig_type = arg;
    if(arg.getTypePtr()->isReferenceType()) {
        data.is_referenced = true;
        arg = arg.getTypePtr()->getPointeeType();
    }
    while(arg.getTypePtr()->isPointerType()) {
        data.pointer_count++;
        arg = arg.getTypePtr()->getPointeeType();
    }
    data.original_type = arg.getAsString();

    auto map_iter = ExportType::type_map.find(arg.getAsString());
    if(map_iter != ExportType::type_map.end()) {
        uint64_t type_size = context->getTypeInfo(arg).Width;
        ostringstream output_str;
        output_str<<map_iter->second.first<<type_size;
        data.c_type = output_str.str();

        data.js_type = map_iter->second.second;
    }
    else if(arg.getAsString()=="void") {
        data.c_type = "void";
        data.js_type = "null";
    }
    else {
        data.c_type = arg.getBaseTypeIdentifier()->getName().str();
        data.js_type = data.c_type;
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
    unit.function_name = funcDecl->getNameInfo().getName().getAsString(); 
    handle_argument(unit.return_data, funcDecl->getCallResultType());
    
    for(auto it=funcDecl->param_begin();it!=funcDecl->param_end();it++) {
        ArgumentData cur_argument;
        handle_argument(cur_argument, (*it)->getType());
        unit.argument.push_back(cur_argument);
    }
    
    
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

void Generator::getParseData(Config &config) {
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

    ParserAction *action = new ParserAction(*this);
    std::vector<std::string> args;
    args.push_back("-m32");

    clang::tooling::runToolOnCodeWithArgs(action, code_str, args);
}

void Generator::genJsFile(std::string &source_name) {
    std::string output_filename = "output/" + source_name + ".js";
    ofstream fs(output_filename);

    std::string hint_str = "";
    std::string entity_str = "";
    for(auto unit_iter=all_unit.begin();unit_iter!=all_unit.end();unit_iter++) {
        std::string name = "";
        std::string arg_refer = "";
        name += (boost::format("_%s") % unit_iter->function_name).str();
        for(auto arg_iter=unit_iter->argument.begin();
                arg_iter!=unit_iter->argument.end();arg_iter++) {
            name += "_";
            if(arg_iter->pointer_count) {
                name += std::to_string(arg_iter->pointer_count);
            }
            name += arg_iter->js_type;
            arg_refer += (boost::format("%d,") % arg_iter->is_referenced).str();
        }
        hint_str += (boost::format("%s: [[%s], %s, %d],\n") 
            % name % arg_refer % unit_iter->return_data.js_type
            % unit_iter->return_data.is_referenced).str();

        entity_str += (boost::format(TemplateSet::js_pure_function_entity)
            % unit_iter->function_name % unit_iter->function_name).str();

    }
    fs<<boost::format(TemplateSet::js_output) % hint_str % entity_str;
    fs.close();
}
