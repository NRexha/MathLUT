#pragma once

#include "LUTSample.h"

#include <memory>
#include <string>
#include <vector>

#include "../Core/Shader.h"

class LUTGenerator
{
public:
    bool GenerateSamples(const std::string& functionSource, std::vector<LUTSample>& samples, float time = 0.0f);
    bool UsesTime(const std::string& functionSource) const;
    void Shutdown();

private:
    std::unique_ptr<Shader> computeShader;
    std::string compiledFunctionSource;
    GLuint outputBuffer = 0;
    static constexpr GLsizei sampleCount = 1024;
};
