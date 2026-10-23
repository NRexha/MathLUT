#pragma once

#include <string>

class ShaderSource
{
public:
    static bool LoadFromFile(const std::string& path, std::string& source);
};
