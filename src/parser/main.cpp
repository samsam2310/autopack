#include <vector>

#include "generator.h"
#include "config.h"

int main(int argc, const char** argv) {
    std::vector<Config> output_config = parseConfig("sample.yaml");
    Generator gen;
    for(auto it=output_config.begin();it!=output_config.end();it++) {
        gen.getParseData(*it);
        for(auto unit_iter=gen.all_unit.begin();unit_iter!=gen.all_unit.end();unit_iter++) {
            std::cerr<<*unit_iter;
        }
        gen.genJsFile(it->filename);
    }
    return 0;
}
