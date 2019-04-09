#include <vector>
#include <string>

#ifndef CONFIG_H
#define CONFIG_H

class FunctionConfig {
public:
    std::string function_name;
    std::vector<std::string> type_list;

    void add_type(std::string &new_type);
};

class Config {
public:
    std::string filename;
    std::vector<FunctionConfig> functions;

    void add_functions(FunctionConfig _function);
};

std::vector<Config> parseConfig(std::string config_filename);

#endif
