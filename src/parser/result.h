#include <iostream>
#include <string>
#include <vector>

#ifndef RESULT_H
#define RESULT_H

class ArgumentData {
public:
    std::string js_type;
    std::string c_type;
    std::string original_type;
    bool is_referenced = false;
    int pointer_count=0;
    friend std::ostream& operator<<(std::ostream& os, const ArgumentData &data) {
        os<<"========\n";
        os<<"js_type: "<<data.js_type<<"\n";
        os<<"c_ptye: "<<data.c_type<<"\n";
        os<<"orig: "<<data.original_type<<"\n";
        os<<"refer: "<<data.is_referenced<<"\n";
        os<<"p_count: "<<data.pointer_count<<"\n";
        os<<"========\n";
        return os;
    }
};

class ExportUnit {
private:

    class JSResultData {
    public:
        std::string name;
    };

    class CPPResultData {
    public:
        std::string return_type;
        std::string function_name;
        std::string entity;
    };
public:
    JSResultData js_data;
    CPPResultData cpp_data;
    ArgumentData return_data;
    std::vector<ArgumentData> argument;
    std::string function_name;
    void printdata() {
        std::cout<<js_data.name<<"\n";
    }
    friend std::ostream& operator<<(std::ostream &os, const ExportUnit &data) {
        os<<"name: "<<data.function_name<<"\n";
        os<<"return data\n"<<data.return_data;
        os<<"args\n";
        for(auto it=data.argument.begin();it!=data.argument.end();it++) {
            os<<*it;
        }
        return os;
    }
};

#endif
