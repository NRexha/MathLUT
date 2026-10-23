#include "ShaderSource.h"

#include <fstream>
#include <iostream>
#include <sstream>

bool ShaderSource::LoadFromFile(const std::string& path, std::string& source)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << path << '\n';
        return false;
    }

    std::ostringstream stream;
    stream << file.rdbuf();

    if (file.bad())
    {
        std::cerr << "Failed to read shader file: " << path << '\n';
        return false;
    }

    source = stream.str();
    return true;
}
