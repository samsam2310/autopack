#include <vector>

#include "config.h"
#include "result.h"

#ifndef GENERATOR_H
#define GENERATOR_H

void genParseResult(int argc, const char** argv, Config &config);

class Generator {
public:
    std::vector<ExportUnit> all_unit;
    void genParseResult(Config &config);
};

#endif
