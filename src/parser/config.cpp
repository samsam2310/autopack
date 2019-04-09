#include <string>
#include <vector>
#include "config.h"
#include "yaml-cpp/yaml.h"

#include <iostream>

std::vector<Config> parseConfig(std::string config_filename) {
    YAML::Node config = YAML::LoadFile(config_filename);
    std::vector<Config> result;
    for(YAML::const_iterator it=config["export"].begin();it!=config["export"].end();it++) {
        Config cur_config;
        if((*it)["filename"]) {
            cur_config.filename = (*it)["filename"].as<std::string>();
        }
        if((*it)["functions"] && !(*it)["functions"].IsNull()) {
            YAML::const_iterator func_it;
            for(func_it=(*it)["functions"].begin();func_it!=(*it)["functions"].end();func_it++) {
                FunctionConfig func_config;
                func_config.function_name = (*func_it)["name"].as<std::string>();
                if((*func_it)["input_types"] && !(*func_it)["input_types"].IsNull()) {
                    for(auto argu_it=(*func_it)["input_types"].begin();
                            argu_it!=(*func_it)["input_types"].end();
                            argu_it++) {
                        func_config.type_list.push_back((*argu_it).as<std::string>());
                    }
                }

                cur_config.functions.push_back(func_config);
            }
        }
        result.push_back(cur_config);
    }
    return result;
}
