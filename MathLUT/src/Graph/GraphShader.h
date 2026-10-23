#pragma once

#include <glad/glad.h>
#include "../Core/ShaderSource.h"
#include <algorithm>
#include <iostream>
#include <string>

namespace GraphShader
{
    inline GLuint Compile(GLenum type, const std::string& source)
    {
        GLuint shader = glCreateShader(type);
        const char* sourcePtr = source.c_str();
        glShaderSource(shader, 1, &sourcePtr, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(static_cast<size_t>(std::max(length, 1)), '\0');
            glGetShaderInfoLog(shader, length, nullptr, log.data());
            std::cerr << "Graph shader compilation failed:\n" << log << '\n';
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    inline GLuint CreateProgram(const std::string& vertexPath, const std::string& fragmentPath)
    {
        std::string vertexSource, fragmentSource;

        if (!ShaderSource::LoadFromFile(vertexPath, vertexSource) || !ShaderSource::LoadFromFile(fragmentPath, fragmentSource))
            return 0;

        GLuint vertexShader = Compile(GL_VERTEX_SHADER, vertexSource);
        GLuint fragmentShader = Compile(GL_FRAGMENT_SHADER, fragmentSource);

        if (!vertexShader || !fragmentShader)
        {
            if (vertexShader) glDeleteShader(vertexShader);
            if (fragmentShader) glDeleteShader(fragmentShader);
            return 0;
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        GLint success = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success)
        {
            GLint length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::string log(static_cast<size_t>(std::max(length, 1)), '\0');
            glGetProgramInfoLog(program, length, nullptr, log.data());
            std::cerr << "Graph shader linking failed:\n" << log << '\n';
            glDeleteProgram(program);
            return 0;
        }

        return program;
    }
}
