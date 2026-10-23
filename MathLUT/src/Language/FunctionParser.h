#pragma once

#include <string>
#include <vector>

struct FunctionStatement
{
    std::string source;
    int lineNumber;
};

class FunctionParser
{
public:
    bool Parse(const std::string& source, std::vector<FunctionStatement>& statements, std::string& error) const;
};