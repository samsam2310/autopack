#include <iostream>
#include <string>
#include <vector>

#ifndef RESULT_H
#define RESULT_H

class ExportUnit {
private:
    class ArgumentData {
        std::string type;
        bool is_referenced;
    };

    class JSResultData {
    public:
        std::string name;
        std::vector<ArgumentData> argument;
        std::string return_type;
        bool is_referenced;
    };

    class CPPResultData {
    public:
        std::string return_type;
        std::string function_name;
        std::string argument;
        std::string entity;
    };
public:
    JSResultData js_data;
    CPPResultData cpp_data;
    void printdata() {
        std::cout<<js_data.name<<"\n";
    }
};

#endif
