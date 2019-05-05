#include <iostream>
#include <string>
#include <vector>

#ifndef RESULT_H
#define RESULT_H

class ArgumentData {
public:
    std::string js_type;
    std::string c_type;
    std::string original_type = "void";
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

class FunctionUnit {
public:
    ArgumentData return_data;
    std::vector<ArgumentData> argument;
    std::string function_name;
    friend std::ostream& operator<<(std::ostream &os, const FunctionUnit &data) {
        os<<"name: "<<data.function_name<<"\n";
        os<<"return data\n"<<data.return_data;
        os<<"args\n";
        for(auto it=data.argument.begin();it!=data.argument.end();it++) {
            os<<*it;
        }
        return os;
    }
};

class FieldUnit {
public:
    ArgumentData arg_data;
    std::string name;
    int offset;
    bool is_self = false;
    // other info
};

class MethodUnit {
public:
    FunctionUnit func_data;
    bool is_constructor = false;
};

class ClassUnit {
public:
    int size;
    std::string class_name;
    std::vector<FieldUnit> fields;
    std::vector<MethodUnit> methods;
    
    
};

#endif
