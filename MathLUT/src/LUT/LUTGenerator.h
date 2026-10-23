#pragma once

#include <string>
#include "LUTSample.h"
#include <vector>


class LUTGenerator
{
public:
    bool RunComputeTest(const std::string& functionSource) const;
    bool GenerateSamples(const std::string& functionSource, std::vector<LUTSample>& samples) const;
};
