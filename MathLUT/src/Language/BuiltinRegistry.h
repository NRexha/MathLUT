#pragma once

#include <string>
#include <vector>

enum class BuiltinType
{
    Existing,
    Constant,
    Uniform
};

struct BuiltinVariable
{
    std::string name;
    std::string glslName;
    BuiltinType type;
    std::string value;
};

class BuiltinRegistry
{
public:
    static const std::vector<BuiltinVariable>& GetAll();
};
