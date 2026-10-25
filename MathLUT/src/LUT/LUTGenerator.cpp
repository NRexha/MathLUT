#include "LUTGenerator.h"

#include "../Core/ShaderSource.h"
#include "../Language/FunctionParser.h"
#include "../Language/GLSLGenerator.h"
#include "../Language/BuiltinRegistry.h"

#include <glad/glad.h>
#include <limits>
#include <regex>

#pragma region Time Detection

bool LUTGenerator::UsesTime(const std::string& functionSource) const
{
    static const std::regex timeIdentifier(R"(\b_Time\b)");
    return std::regex_search(functionSource, timeIdentifier);
}

#pragma endregion

#pragma region Sample Generation

bool LUTGenerator::GenerateSamples(const std::string& functionSource, std::vector<LUTSample>& samples, float time, std::string* error)
{
    if (error)
        error->clear();

    if (!computeShader || compiledFunctionSource != functionSource)
    {
        std::string shaderTemplate, generatedSource, generationError;

        if (!ShaderSource::LoadFromFile("src/Shaders/LUTSample.comp", shaderTemplate))
        {
            if (error)
                *error = "Failed to load LUTSample.comp.";

            return false;
        }

        FunctionParser parser;
        std::vector<FunctionStatement> statements;

        if (!parser.Parse(functionSource, statements, generationError))
        {
            if (error)
                *error = "Function parsing failed:\n" + generationError;

            return false;
        }

        GLSLGenerator generator;

        if (!generator.Generate(shaderTemplate, statements, generatedSource, generationError))
        {
            if (error)
                *error = "GLSL generation failed:\n" + generationError;

            return false;
        }

        auto newShader = std::make_unique<Shader>(generatedSource.c_str());

        if (!newShader->IsValid())
        {
            if (error)
                *error = newShader->GetError();

            return false;
        }

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
        if (error)
            *error = "Compute shader is missing the sampleCount uniform.";

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

#pragma endregion

#pragma region Texture Generation

bool LUTGenerator::GenerateTexture(const std::string& functionSource, int width, int height, std::vector<LUTSample>& samples, std::string* error)
{
    if (error)
        error->clear();

    if (width <= 0 || height <= 0)
    {
        if (error)
            *error = "Texture dimensions must be greater than zero.";

        return false;
    }

    const size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);

    if (pixelCount > static_cast<size_t>(std::numeric_limits<GLsizeiptr>::max()) / sizeof(LUTSample))
    {
        if (error)
            *error = "Texture dimensions exceed the supported buffer size.";

        return false;
    }

    if (!textureComputeShader || compiledTextureFunctionSource != functionSource)
    {
        std::string shaderTemplate, generatedSource, generationError;

        if (!ShaderSource::LoadFromFile("src/Shaders/TexturePreview.comp", shaderTemplate))
        {
            if (error)
                *error = "Failed to load TexturePreview.comp.";

            return false;
        }

        FunctionParser parser;
        std::vector<FunctionStatement> statements;

        if (!parser.Parse(functionSource, statements, generationError))
        {
            if (error)
                *error = "Function parsing failed:\n" + generationError;

            return false;
        }

        GLSLGenerator generator;

        if (!generator.Generate(shaderTemplate, statements, generatedSource, generationError))
        {
            if (error)
                *error = "GLSL generation failed:\n" + generationError;

            return false;
        }

        auto newShader = std::make_unique<Shader>(generatedSource.c_str());

        if (!newShader->IsValid())
        {
            if (error)
                *error = newShader->GetError();

            return false;
        }

        textureComputeShader = std::move(newShader);
        compiledTextureFunctionSource = functionSource;
    }

    if (textureOutputBuffer == 0)
        glGenBuffers(1, &textureOutputBuffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureOutputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(pixelCount * sizeof(LUTSample)), nullptr, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, textureOutputBuffer);

    textureComputeShader->Use();

    const GLint widthLocation = glGetUniformLocation(textureComputeShader->GetProgramID(), "textureWidth");
    const GLint heightLocation = glGetUniformLocation(textureComputeShader->GetProgramID(), "textureHeight");

    if (widthLocation < 0 || heightLocation < 0)
    {
        if (error)
            *error = "Texture compute shader is missing a resolution uniform.";

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        return false;
    }

    glUniform1i(widthLocation, width);
    glUniform1i(heightLocation, height);

    for (const BuiltinVariable& builtin : BuiltinRegistry::GetAll())
    {
        if (builtin.type != BuiltinType::Uniform || builtin.name != "_Time")
            continue;

        const GLint location = glGetUniformLocation(textureComputeShader->GetProgramID(), builtin.glslName.c_str());
        if (location >= 0)
            glUniform1f(location, 0.0f);
    }

    glDispatchCompute((width + 7) / 8, (height + 7) / 8, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    samples.resize(pixelCount);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureOutputBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, static_cast<GLsizeiptr>(pixelCount * sizeof(LUTSample)), samples.data());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return true;
}

#pragma endregion

#pragma region Shutdown

void LUTGenerator::Shutdown()
{
    if (outputBuffer != 0)
    {
        glDeleteBuffers(1, &outputBuffer);
        outputBuffer = 0;
    }

    if (textureOutputBuffer != 0)
    {
        glDeleteBuffers(1, &textureOutputBuffer);
        textureOutputBuffer = 0;
    }

    computeShader.reset();
    compiledFunctionSource.clear();

    textureComputeShader.reset();
    compiledTextureFunctionSource.clear();
}

#pragma endregion
