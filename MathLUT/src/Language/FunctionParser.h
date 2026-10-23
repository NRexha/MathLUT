#pragma once

#include <string>
#include <vector>

struct FunctionAssignment
{
    std::string channel;
    std::string expression;
};

class FunctionParser
{
public:
    bool Parse(const std::string& source, std::vector<FunctionAssignment>& assignments, std::string& error) const;
};
