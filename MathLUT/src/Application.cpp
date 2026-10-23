#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "LUT/LUTGenerator.h"
#include "Graph/GraphView.h"
#include "LUT/LUTSample.h"
#include "Core/ShaderSource.h"

#include <vector>
#include <iostream>

int main()
{
#pragma region InitGLFW
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
#pragma endregion

#pragma region InitGLAD
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD.\n";

        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
#pragma endregion

#pragma region GraphView
    GraphView graphView;

    if (!graphView.Initialize(window))
    {
        std::cerr << "Failed to initialize graph view.\n";
        glfwTerminate();
        return -1;
    }
#pragma endregion

#pragma region WindowSettings
    glfwSwapInterval(1); //enable VSync
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
#pragma endregion

#pragma region LUT
    LUTGenerator lutGenerator;

    std::string functionSource;

    if (!ShaderSource::LoadFromFile("src/Functions/TestFunction.mlut", functionSource))
    {
        std::cerr << "Failed to load test function.\n";
        return -1;
    }

    std::vector<LUTSample> samples;

    if (lutGenerator.GenerateSamples(functionSource, samples))
        graphView.SetSamples(samples);

    if (!lutGenerator.RunComputeTest(functionSource))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
#pragma endregion

    //render loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.12f, 0.13f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();
        graphView.Update();
        graphView.Render();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
