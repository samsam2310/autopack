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
        handleCXXRecordDecl(recordDecl);
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

void ParserHandler::handleCXXRecordDecl(const CXXRecordDecl *recordDecl) {
    std::cerr<<"==record decl==\n";
    if(!(
        recordDecl->hasDefinition() &&
        !recordDecl->isCXXClassMember())) {
        std::cerr<<"==skipped==\n";
        return;
    }
    recordDecl->dump();
    const clang::ASTRecordLayout& layout(context->getASTRecordLayout(recordDecl));
    ClassUnit class_data;
    class_data.class_name = recordDecl->getName().str();
    class_data.size = layout.getSize().getQuantity();
    for(auto it=recordDecl->field_begin();it!=recordDecl->field_end();it++) {
        FieldUnit field_data;
        unsigned field_index = (*it)->getFieldIndex();
        handle_argument(field_data.arg_data, (*it)->getType());
        field_data.offset = layout.getFieldOffset(field_index)/8;
        field_data.name = (*it)->getName().str();
        class_data.fields.push_back(field_data);
    }
    for(auto it=recordDecl->method_begin();it!=recordDecl->method_end();it++) {
        if(!(
            !(*it)->isTrivial() &&
            !isa<CXXConstructorDecl>(*it) &&
            !(*it)->isDefaulted())) {
            continue;
        }
        std::cerr<<(*it)->getNameInfo().getName().getAsString()<<"\n";
        MethodUnit method_data;
        method_data.func_data.function_name = (*it)->getNameInfo().getName().getAsString();
        handle_argument(method_data.func_data.return_data, (*it)->getCallResultType());
        for(auto arg_it=(*it)->param_begin();arg_it!=(*it)->param_end();arg_it++) {
            ArgumentData arg_data;
            handle_argument(arg_data, (*arg_it)->getType());
            method_data.func_data.argument.push_back(arg_data);
        }

        class_data.methods.push_back(method_data);
    }
    std::cerr<<"constructor\n";
    for(auto it=recordDecl->ctor_begin();it!=recordDecl->ctor_end();it++) {
        std::cerr<<(*it)->getNameInfo().getName().getAsString()<<"\n";
        MethodUnit method_data;
        method_data.is_constructor = true;
        for(auto arg_it=(*it)->param_begin();arg_it!=(*it)->param_end();arg_it++) {
            ArgumentData arg_data;
            handle_argument(arg_data, (*arg_it)->getType());
            method_data.func_data.argument.push_back(arg_data);
        }

        class_data.methods.push_back(method_data);
    }
    _generator.class_units.push_back(class_data);

    
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

    FunctionUnit unit;
    unit.function_name = funcDecl->getNameInfo().getName().getAsString(); 
    handle_argument(unit.return_data, funcDecl->getCallResultType());
    for(auto it=funcDecl->param_begin();it!=funcDecl->param_end();it++) {
        ArgumentData cur_argument;
        handle_argument(cur_argument, (*it)->getType());
        unit.argument.push_back(cur_argument);
    }
    
    
    _generator.function_units.push_back(unit);
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

void Generator::genArgString(
        std::string &name,
        std::string &arg_refer,
        std::string &c_define_args,
        std::string &c_call_args,
        std::string &c_return_type,
        FunctionUnit &function_data) {
    int var_index = 0;
    name += function_data.function_name;
    for(auto arg_iter=function_data.argument.begin();
            arg_iter!=function_data.argument.end();
            arg_iter++, var_index++) {
        name += "_";
        if(arg_iter->pointer_count) {
            name += std::to_string(arg_iter->pointer_count);
        }
        name += arg_iter->c_type;
        arg_refer += (boost::format("%d,") % arg_iter->is_referenced).str();

        c_define_args += arg_iter->original_type;
        for(int i=0;i<=arg_iter->pointer_count;i++) {
            c_define_args += "*";
        }
        c_define_args += "a"+to_string(var_index);
        c_call_args += "*a" + to_string(var_index);
        if(arg_iter+1!=function_data.argument.end()) {
            c_define_args += ",";
            c_call_args += ",";
        }
    }
    if(function_data.return_data.original_type != "void") {
        c_return_type = "char*";
    }
    else {
        c_return_type = "void";
    }
}

void Generator::genClassEntity(std::string &js_hint_str, std::string &js_entity_str,
        std::string &c_entity_str) {
    for(auto class_iter=class_units.begin();class_iter!=class_units.end();class_iter++) {
        std::string class_constructor_entity = "";
        std::string class_field_entity = "";
        for(auto method_iter=class_iter->methods.begin();
                method_iter!=class_iter->methods.end();method_iter++) {
            std::string name = "";
            std::string arg_refer = "";
            std::string c_define_args = "";
            std::string c_call_args = "";
            std::string c_return_type = "";
            genArgString(name, arg_refer, c_define_args, c_call_args,
                c_return_type, method_iter->func_data);

            //output example  obj_func(obj_type *obj [, c_define_args])
            if(c_define_args != "") {
                c_define_args = "," + c_define_args;
            }
            if(method_iter->is_constructor) {
                name = (boost::format("construct_%s%s") % class_iter->class_name % name).str();
                class_constructor_entity += (boost::format("_%s: [[1,%s], null, 0],\n")
                    % name % arg_refer).str();

                c_entity_str += (boost::format(TemplateSet::c_class_constructor_entity)
                    % c_return_type % name % class_iter->class_name
                    % c_define_args % class_iter->class_name % c_call_args).str();
            }
            else {
                name = (boost::format("%s_") % class_iter->class_name).str() + name;
                js_hint_str += (boost::format("_%s: [[1,%s], %s, %d],\n")
                    % name % arg_refer % method_iter->func_data.return_data.js_type
                    % method_iter->func_data.return_data.is_referenced).str();
                js_entity_str += (boost::format(TemplateSet::js_class_method_entity)
                    % class_iter->class_name % method_iter->func_data.function_name
                    % name).str();


                c_entity_str += (boost::format(TemplateSet::c_class_method_entity)
                    % c_return_type % name % class_iter->class_name
                    % c_define_args % method_iter->func_data.function_name
                    % c_call_args).str();
            }

        }
        for(auto field_iter=class_iter->fields.begin();
                field_iter!=class_iter->fields.end();field_iter++) {
            class_field_entity += (boost::format("'%s':{type:%s,offset:%d},\n")
                % field_iter->name % field_iter->arg_data.js_type % field_iter->offset).str();
        }
        js_entity_str += (boost::format(TemplateSet::js_class_entity)
            % class_iter->class_name % class_iter->class_name
            % class_field_entity % class_iter->size % class_constructor_entity).str();
    } 

}

void Generator::genResultFile(std::string &source_name) {
    std::string js_output_filename = "output/" + source_name + ".js";
    std::string c_output_filename = "output/" + source_name + ".cpp";
    ofstream js_fs(js_output_filename);
    ofstream c_fs(c_output_filename);
    

    std::string js_hint_str = "";
    std::string js_entity_str = "";
    std::string c_entity_str = "";
    for(auto unit_iter=function_units.begin();unit_iter!=function_units.end();unit_iter++) {
        std::string name = "";
        std::string arg_refer = "";
        std::string c_define_args = "";
        std::string c_call_args = "";
        std::string c_return_type = "";
        genArgString(name, arg_refer, c_define_args, c_call_args, c_return_type, *unit_iter);

        js_hint_str += (boost::format("_%s: [[%s], %s, %d],\n") 
            % name % arg_refer % unit_iter->return_data.js_type
            % unit_iter->return_data.is_referenced).str();

        js_entity_str += (boost::format(TemplateSet::js_pure_function_entity)
            % unit_iter->function_name % unit_iter->function_name).str();


        /*c_return_type += unit_iter->return_data.original_type;
        for(int i=0;i<unit_iter->return_data.pointer_count;i++) {
            c_return_type += "*";
        }*/
        c_entity_str += (boost::format(TemplateSet::c_pure_function_entity)
            % c_return_type % name % c_define_args % unit_iter->function_name % c_call_args).str();

    }
    genClassEntity(js_hint_str, js_entity_str, c_entity_str);
    js_fs<<boost::format(TemplateSet::js_output) % js_hint_str % js_entity_str;
    c_fs<<boost::format(TemplateSet::c_output) % c_entity_str;
    js_fs.close();
    c_fs.close();
}
