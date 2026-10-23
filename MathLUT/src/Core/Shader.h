#pragma once

#include <glad/glad.h>

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

private:
    GLuint programID = 0;
};
