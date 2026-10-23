#pragma once

#include "FunctionParser.h"

#include <string>
#include <vector>

class GLSLGenerator
{
public:
    bool Generate(const std::string& shaderTemplate, const std::vector<FunctionStatement>& statements, std::string& generatedSource, std::string& error) const;
};