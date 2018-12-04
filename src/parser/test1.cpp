#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "clang/AST/AST.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace std;
using namespace llvm;

static cl::OptionCategory MyToolCategory("my-tool options");

int i = 1;

std::map<string, string> type_map = {
    {"short", "i"},
    {"short int", "i"},
    {"signed short", "i"},
    {"signed short int", "i"},
    {"unsigned short", "u"},
    {"unsigned short int", "u"},
    {"int", "i"},
    {"signed", "i"},
    {"signed int", "i"},
    {"unsigned", "u"},
    {"unsigned int", "u"},
    {"long", "i"},
    {"long int", "i"},
    {"signed long", "i"},
    {"signed long int", "i"},
    {"unsigned long", "u"},
    {"unsigned long int", "u"},
    {"long long", "i"},
    {"long long int", "i"},
    {"signed long long", "i"},
    {"signed long long int", "i"},
    {"unsigned long long", "u"},
    {"unsigned long long int", "u"}
};

std::map<string, string> js_type_map = {
    {"short", "CShort"},
    {"short int", "CShort"},
    {"signed short", "CShort"},
    {"signed short int", "CShort"},
    {"unsigned short", "CUnsignedShort"},
    {"unsigned short int", "CUnsignedShort"},
    {"int", "CLong"},
    {"signed", "CLong"},
    {"signed int", "CLong"},
    {"unsigned", "CUnsignedLong"},
    {"unsigned int", "CUnsignedLong"},
    {"long", "CLong"},
    {"long int", "CLong"},
    {"signed long", "CLong"},
    {"signed long int", "CLong"},
    {"unsigned long", "CUnsignedLong"},
    {"unsigned long int", "CUnsignedLong"},
    {"long long", "CLongLong"},
    {"long long int", "CLongLong"},
    {"signed long long", "CLongLong"},
    {"signed long long int", "CLongLong"},
    {"unsigned long long", "CUnsignedLongLong"},
    {"unsigned long long int", "CUnsignedLongLong"}
};

std::map<string, int> js_func_exist;

ofstream cpp_result;
ofstream js_result;

string js_hint = "";
string js_content = "";

class FuncPrinter : public MatchFinder::MatchCallback {
public:
    virtual void run(const MatchFinder::MatchResult &Result) {
        const FunctionDecl* funcDecl = Result.Nodes.getNodeAs<clang::FunctionDecl>("func");
        FunctionDecl::param_const_iterator it;
        funcDecl->dump();
        std::cerr<<"Name: "<<funcDecl->getNameInfo().getName().getAsString()<<"\n";
        std::cerr<<"Type: "<<funcDecl->getCallResultType().getAsString()<<"\n";
        std::cerr<<"\n";
        std::cerr<<"First Declare: ";
        std::cerr<<funcDecl->isFirstDecl()<<"\n";
        std::cerr<<"Is Main: "<<funcDecl->isMain()<<"\n";

        if(!(funcDecl->isFirstDecl() && ! funcDecl->isMain())) {
            // if not first decl or is main, pass
            return;
        }

        std::cerr<<"Param Type: ";
        for(it=funcDecl->param_begin();it!=funcDecl->param_end();it++) {
            string argument_type = (*it)->getOriginalType().getAsString();
            std::cerr<<argument_type<<"\n";
        }

        this->generateCPPResult(Result, cpp_result);
    }
private:
    void generateCPPResult(const MatchFinder::MatchResult &Result, ofstream &output_stream) {
        const FunctionDecl* funcDecl = Result.Nodes.getNodeAs<clang::FunctionDecl>("func");
        FunctionDecl::param_const_iterator it;

        output_stream<<"EMSCRIPTEN_KEEPALIVE\n";
        output_stream<<funcDecl->getCallResultType().getAsString()<<" "
            <<funcDecl->getNameInfo().getName().getAsString();

        js_hint += "_" + funcDecl->getNameInfo().getName().getAsString();
        std::map<string, string>::iterator iter;
        iter = type_map.find(funcDecl->getCallResultType().getAsString());
        if(iter != type_map.end()) {
            uint64_t type_size = funcDecl->getASTContext().getTypeInfo(funcDecl->getCallResultType()).Width;
            js_hint += "_" + iter->second + std::to_string(type_size);
        }
        else {
        }
        js_hint += ": [[";

        std::map<string,int>::iterator func_iter;
        func_iter = js_func_exist.find(funcDecl->getNameInfo().getName().getAsString());
        if(func_iter == js_func_exist.end()) {
            js_content += "C." + funcDecl->getNameInfo().getName().getAsString() + " = (...args) => {\n";
            js_content += "const ident = getCFuncIdent(wasmModule, '_" +
                funcDecl->getNameInfo().getName().getAsString() + "', ...args);\n";
            js_content += "const hint = hints[ident];\n"
                          "return callCFunction(\n"
                          "wasmModule,\n"
                          "hint[0],\n"
                          "ident,\n"
                          "hint[1],\n"
                          "hint[2],\n"
                          "...args\n"
                          ");\n"
                          "};\n";
            js_func_exist[funcDecl->getNameInfo().getName().getAsString()] = 1;
        }

        for(it=funcDecl->param_begin();it!=funcDecl->param_end();it++) {
            string argument_type = (*it)->getOriginalType().getAsString();
            
            std::map<string, string>::iterator iter;
            iter = type_map.find(argument_type);
            // is basic type
            if(iter != type_map.end()) {
                uint64_t type_size = (*it)->getASTContext().getTypeInfo((*it)->getOriginalType()).Width;
                output_stream<<"_"<<iter->second<<type_size;
            }
            else {
            }
        }
        output_stream<<"(";
        int serial_num = 0;
        vector<int> star_count;
        for(it=funcDecl->param_begin();it!=funcDecl->param_end();it++, serial_num++) {
            int count = 1;
            int is_reference = 0;
            int i;
            if(serial_num) {
                output_stream<<", ";
            }
            string argument_type = (*it)->getOriginalType().getAsString();
            std::vector<string> v;
            string type_base = "";
            for(i=0;argument_type[i]!=' ' && argument_type[i]!='\0';i++) {
                type_base += argument_type[i];
            }
            output_stream<<type_base<<" *";
            for(;argument_type[i];i++) {
                if(argument_type[i]=='*') {
                    count++;
                    output_stream<<"*";
                }
                if(argument_type[i] == '&') {
                    is_reference = 1;
                }
            }
            star_count.push_back(count);
            output_stream<<"a"<<serial_num;

            js_hint += std::to_string(is_reference) + ",";
        }        
        output_stream<<") {\n";
        output_stream<<"\treturn "<<funcDecl->getNameInfo().getName().getAsString()<<"(";

        js_hint += "], ";
        iter = js_type_map.find(funcDecl->getCallResultType().getAsString());
        if(iter != js_type_map.end()) {
            js_hint += iter->second;
        }
        else {
        }

        js_hint += ", 0],\n";

        serial_num = 0;
        for(vector<int>::iterator v_iter=star_count.begin();
                v_iter!=star_count.end();v_iter++, serial_num++) {
            if(serial_num) {
                output_stream<<", ";
            }
            for(i=0;i<*v_iter;i++) {
                output_stream<<"*";
            }
            output_stream<<"a"<<serial_num;
            
        }
        output_stream<<");\n";
        output_stream<<"}\n";
    }

};

void cppPreProcess() {
    // print header
    cpp_result<<"#include <emscripten.h>\n";
    cpp_result<<"extern \"C\" {\n";
}

void cppPostProcess() {
    cpp_result<<"}\n";
}

void jsGenerate() {
    js_result<<"import { CLong, CFloat, CDouble } from './types';\n"
               "import { callCFunction, getCFuncIdent } from './wrap';\n"
               "import { Module } from './module';\n";

    js_result<<"class TestModule extends Module {\n"
               "    initCMethod() {\n"
               "        const wasmModule = this._wasmModule;\n"
               "        const C = {};\n"
               "        const hints = {\n";
    js_result<<js_hint;
    js_result<<"        };\n";
    js_result<<js_content;
    js_result<<"        this.C = C;\n"
               "    }\n"
               "}\n"
               "export { TestModule };";
}

int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

    DeclarationMatcher funcMatcher = functionDecl().bind("func");
    FuncPrinter printer;
    MatchFinder finder;
    finder.addMatcher(funcMatcher, &printer);

    cpp_result.open("result.cpp");
    cppPreProcess();
    int result = Tool.run(newFrontendActionFactory(&finder).get());
    cppPostProcess();
    cpp_result.close();

    js_result.open("result.js");
    jsGenerate();
    return result;
}

