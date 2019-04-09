#include <vector>

#include "generator.h"
#include "config.h"

int main(int argc, const char** argv) {
    std::vector<Config> output_config = parseConfig("sample.yaml");
    for(auto it=output_config.begin();it!=output_config.end();it++) {
        genParseResult(argc, argv, *it);
    }
    return 0;
}
