#include <vector>

#include "config.h"
#include "result.h"

#ifndef GENERATOR_H
#define GENERATOR_H

void genParseResult(int argc, const char** argv, Config &config);

class Generator {
public:
    std::vector<FunctionUnit> function_units;
    std::vector<ClassUnit> class_units;
    void getParseData(Config &config);
    void genResultFile(std::string &source_name);
private:
    void genClassEntity(std::string &js_hint_str, std::string &js_entity_str,
        std::string &c_entity_str);
    void genArgString(std::string &name, std::string &arg_refer, std::string &c_define_args,
        std::string &c_call_args, std::string &c_return_type, FunctionUnit &function_data);
};

#endif
