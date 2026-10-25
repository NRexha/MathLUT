#include "Shader.h"

#include <utility>
#include <vector>

#pragma region Compute Shader Compilation

namespace
{
    GLuint CompileComputeShader(const char* source, std::string& errorMessage)
    {
        GLuint shader = glCreateShader(GL_COMPUTE_SHADER);

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            GLint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            std::vector<char> log(logLength > 0 ? logLength : 1);
            glGetShaderInfoLog(shader, logLength, nullptr, log.data());

            errorMessage = "Compute shader compilation failed:\n" + std::string(log.data());

            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }
}

#pragma endregion

#pragma region Construction and Destruction

Shader::Shader(const char* computeSource)
{
    const GLuint shader = CompileComputeShader(computeSource, errorMessage);

    if (shader == 0)
        return;

    programID = glCreateProgram();

    glAttachShader(programID, shader);
    glLinkProgram(programID);

    GLint success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLint logLength = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> log(logLength > 0 ? logLength : 1);
        glGetProgramInfoLog(programID, logLength, nullptr, log.data());

        errorMessage = "Compute program linking failed:\n" + std::string(log.data());

        glDeleteProgram(programID);
        programID = 0;
    }

    if (programID != 0)
        glDetachShader(programID, shader);

    glDeleteShader(shader);
}

Shader::~Shader()
{
    if (programID != 0)
        glDeleteProgram(programID);
}

#pragma endregion

#pragma region Move Operations

Shader::Shader(Shader&& other) noexcept
    : programID(std::exchange(other.programID, 0)),
    errorMessage(std::move(other.errorMessage))
{
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this == &other)
        return *this;

    if (programID != 0)
        glDeleteProgram(programID);

    programID = std::exchange(other.programID, 0);
    errorMessage = std::move(other.errorMessage);

    return *this;
}

#pragma endregion

#pragma region Program Access

void Shader::Use() const
{
    if (programID != 0)
        glUseProgram(programID);
}

GLuint Shader::GetProgramID() const
{
    return programID;
}

bool Shader::IsValid() const
{
    return programID != 0;
}

const std::string& Shader::GetError() const
{
    return errorMessage;
}

#pragma endregion
