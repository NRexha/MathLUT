#pragma once

#include <glad/glad.h>

#include <string>

class Shader
{
public:
    explicit Shader(const char* computeSource);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void Use() const;

    GLuint GetProgramID() const;
    bool IsValid() const;
    const std::string& GetError() const;

private:
    GLuint programID = 0;
    std::string errorMessage;
};
