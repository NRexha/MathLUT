#include "LUTGenerator.h"

#include "../Core/ShaderSource.h"
#include "../Language/FunctionParser.h"
#include "../Language/GLSLGenerator.h"
#include "../Language/BuiltinRegistry.h"

#include <glad/glad.h>
#include <iostream>
#include <regex>

bool LUTGenerator::UsesTime(const std::string& functionSource) const
{
    static const std::regex timeIdentifier(R"(\b_Time\b)");
    return std::regex_search(functionSource, timeIdentifier);
}

bool LUTGenerator::GenerateSamples(const std::string& functionSource, std::vector<LUTSample>& samples, float time)
{
    if (!computeShader || compiledFunctionSource != functionSource)
    {
        std::string shaderTemplate, generatedSource, error;

        if (!ShaderSource::LoadFromFile("src/Shaders/TestCompute.comp", shaderTemplate))
            return false;

        FunctionParser parser;
        std::vector<FunctionStatement> statements;

        if (!parser.Parse(functionSource, statements, error))
        {
            std::cerr << "Function parsing failed:\n" << error << '\n';
            return false;
        }

        GLSLGenerator generator;

        if (!generator.Generate(shaderTemplate, statements, generatedSource, error))
        {
            std::cerr << "GLSL generation failed:\n" << error << '\n';
            return false;
        }

        auto newShader = std::make_unique<Shader>(generatedSource.c_str());

        if (!newShader->IsValid())
            return false;

        computeShader = std::move(newShader);
        compiledFunctionSource = functionSource;
    }

    if (outputBuffer == 0)
    {
        glGenBuffers(1, &outputBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(sampleCount * sizeof(LUTSample)), nullptr, GL_DYNAMIC_READ);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, outputBuffer);
    }

    computeShader->Use();

    const GLint countLocation = glGetUniformLocation(computeShader->GetProgramID(), "sampleCount");
    if (countLocation < 0)
    {
        std::cerr << "Compute shader is missing the sampleCount uniform.\n";
        return false;
    }

    glUniform1i(countLocation, sampleCount);

    for (const BuiltinVariable& builtin : BuiltinRegistry::GetAll())
    {
        if (builtin.type != BuiltinType::Uniform || builtin.name != "_Time")
            continue;

        const GLint location = glGetUniformLocation(computeShader->GetProgramID(), builtin.glslName.c_str());
        if (location >= 0)
            glUniform1f(location, time);
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, outputBuffer);
    glDispatchCompute(sampleCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    samples.resize(sampleCount);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, static_cast<GLsizeiptr>(samples.size() * sizeof(LUTSample)), samples.data());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return true;
}

void LUTGenerator::Shutdown()
{
    if (outputBuffer != 0)
    {
        glDeleteBuffers(1, &outputBuffer);
        outputBuffer = 0;
    }

    computeShader.reset();
    compiledFunctionSource.clear();
}
