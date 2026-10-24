#pragma once

#include "LUTSample.h"
#include "../Core/Shader.h"

#include <memory>
#include <string>
#include <vector>

class LUTGenerator
{
public:
    //curve preview
    bool GenerateSamples(const std::string& functionSource, std::vector<LUTSample>& samples, float time = 0.0f);
    bool UsesTime(const std::string& functionSource) const;

    //texture preview
    bool GenerateTexture(const std::string& functionSource, int width, int height, std::vector<LUTSample>& samples);

    //resource cleanup
    void Shutdown();

private:
    std::unique_ptr<Shader> computeShader;
    std::string compiledFunctionSource;
    GLuint outputBuffer = 0;

    std::unique_ptr<Shader> textureComputeShader;
    std::string compiledTextureFunctionSource;
    GLuint textureOutputBuffer = 0;

    static constexpr GLsizei sampleCount = 1024;
};
