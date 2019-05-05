#include <vector>

#include "generator.h"
#include "config.h"

int main(int argc, const char** argv) {
    std::vector<Config> output_config = parseConfig("sample.yaml");
    Generator gen;
    for(auto it=output_config.begin();it!=output_config.end();it++) {
        gen.getParseData(*it);
        gen.genResultFile(it->filename);
    }
    return 0;
}
