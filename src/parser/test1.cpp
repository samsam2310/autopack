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

ofstream cpp_result;
ofstream js_result;

class FuncPrinter : public MatchFinder::MatchCallback {
public:
    virtual void run(const MatchFinder::MatchResult &Result) {
        const FunctionDecl* funcDecl = Result.Nodes.getNodeAs<clang::FunctionDecl>("func");
        FunctionDecl::param_const_iterator it;
        //funcDecl->dump();
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
            }
            star_count.push_back(count);
            output_stream<<"a"<<serial_num;
 
            
        }
        output_stream<<") {\n";
        output_stream<<"\treturn "<<funcDecl->getNameInfo().getName().getAsString()<<"(";
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
    return result;
}

