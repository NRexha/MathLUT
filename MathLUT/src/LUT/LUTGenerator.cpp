#include "LUTGenerator.h"

#include "../Core/Shader.h"
#include "../Core/ShaderSource.h"
#include "../Language/FunctionParser.h"
#include "../Language/GLSLGenerator.h"
#include "LUTSample.h"

#include <glad/glad.h>
#include <iostream>
#include <vector>

bool LUTGenerator::RunComputeTest(const std::string& functionSource) const
{
    //load compute shader
    std::string shaderTemplate;
    if (!ShaderSource::LoadFromFile("src/Shaders/TestCompute.comp", shaderTemplate))
        return false;

    //parse user function
    FunctionParser parser;
    std::vector<FunctionAssignment> assignments;
    std::string error;

    if (!parser.Parse(functionSource, assignments, error))
    {
        std::cerr << "Function parsing failed:\n" << error << '\n';
        return false;
    }

    //insert assignments into shader
    GLSLGenerator generator;
    std::string generatedSource;

    if (!generator.Generate(shaderTemplate, assignments, generatedSource, error))
    {
        std::cerr << "GLSL generation failed:\n" << error << '\n';
        return false;
    }

    //compile and link the generated shader
    Shader computeShader(generatedSource.c_str());
    if (!computeShader.IsValid())
        return false;

    //temp output buffer
    constexpr int sampleCount = 8;
    std::vector<float> results(sampleCount * 4, 0.0f);
    GLuint outputBuffer = 0;
    glGenBuffers(1, &outputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(results.size() * sizeof(float)), nullptr, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, outputBuffer);

    //run shader
    computeShader.Use();
    const GLint location = glGetUniformLocation(computeShader.GetProgramID(), "sampleCount");
    glUniform1i(location, sampleCount);
    glDispatchCompute(sampleCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    //read result
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, static_cast<GLsizeiptr>(results.size() * sizeof(float)), results.data());

    std::cout << "\nResults:\n";
    std::cout << "-------------------\n";

    for (int i = 0; i < sampleCount; ++i)
    {
        const int offset = i * 4;
        std::cout << "Sample " << i << " | R: " << results[offset] << " | G: " << results[offset + 1] << " | B: " << results[offset + 2] << " | A: " << results[offset + 3] << '\n';
    }

    //release
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glDeleteBuffers(1, &outputBuffer);
    return true;
}

bool LUTGenerator::GenerateSamples(const std::string& functionSource, std::vector<LUTSample>& samples) const
{
    constexpr GLsizei sampleCount = 1024;
    std::string shaderTemplate, generatedSource, error;

    //load compute shader
    if (!ShaderSource::LoadFromFile("src/Shaders/TestCompute.comp", shaderTemplate))
        return false;

    //parse user function
    FunctionParser parser;
    std::vector<FunctionAssignment> assignments;

    if (!parser.Parse(functionSource, assignments, error))
    {
        std::cerr << "Function parsing failed:\n" << error << '\n';
        return false;
    }

    //insert assignments into shader
    GLSLGenerator generator;

    if (!generator.Generate(shaderTemplate, assignments, generatedSource, error))
    {
        std::cerr << "GLSL generation failed:\n" << error << '\n';
        return false;
    }

    //compile and link the generated shader
    Shader computeShader(generatedSource.c_str());

    if (!computeShader.IsValid())
        return false;

    //allocate output buffer
    GLuint outputBuffer = 0;
    glGenBuffers(1, &outputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(sampleCount * sizeof(LUTSample)), nullptr, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, outputBuffer);

    //run shader
    computeShader.Use();
    const GLint location = glGetUniformLocation(computeShader.GetProgramID(), "sampleCount");

    if (location < 0)
    {
        std::cerr << "Compute shader is missing the sampleCount uniform.\n";
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        glDeleteBuffers(1, &outputBuffer);
        return false;
    }

    glUniform1i(location, sampleCount);
    glDispatchCompute(sampleCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    //read samples
    samples.resize(sampleCount);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, static_cast<GLsizeiptr>(samples.size() * sizeof(LUTSample)), samples.data());

    //release
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glDeleteBuffers(1, &outputBuffer);
    return true;
}
