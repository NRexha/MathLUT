#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "LUT/LUTGenerator.h"

#include <iostream>

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "MathLUT", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD.\n";

        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwSwapInterval(1);

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';

    LUTGenerator lutGenerator;

    //test user function
    const std::string functionSource = R"(
        r = x;
        g = x * x;
        b = 1.0 - x;
        a = 1.0;
    )";

    if (!lutGenerator.RunComputeTest(functionSource))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.12f, 0.13f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
