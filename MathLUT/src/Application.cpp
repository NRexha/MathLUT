#include <glad/glad.h>
#include "Application.h"

#include <algorithm>
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

int Application::Run()
{
    if (!Initialize())
    {
        Shutdown();
        return -1;
    }

    RunLoop();
    Shutdown();
    return 0;
}

bool Application::Initialize()
{
    if (!InitializeWindow())
        return false;

    if (!graphView.Initialize(window))
    {
        std::cerr << "Failed to initialize graph view.\n";
        return false;
    }

    graphInitialized = true;

    if (!InitializeImGui())
        return false;

    glfwSwapInterval(1);
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';

    if (!functionEditorPanel.Initialize(lutGenerator, graphView))
        return false;

    return true;
}

bool Application::InitializeWindow()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW.\n";
        return false;
    }

    glfwInitialized = true;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "MathLUT", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window.\n";
        return false;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD.\n";
        return false;
    }

    return true;
}

bool Application::InitializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    imguiContextCreated = true;

    ImGui::GetIO().FontGlobalScale = 1.25f;
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
    {
        std::cerr << "Failed to initialize ImGui GLFW backend.\n";
        return false;
    }

    imguiGlfwInitialized = true;

    if (!ImGui_ImplOpenGL3_Init("#version 430"))
    {
        std::cerr << "Failed to initialize ImGui OpenGL backend.\n";
        return false;
    }

    imguiOpenGLInitialized = true;
    return true;
}

void Application::RunLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int windowWidth, windowHeight;
        int framebufferWidth, framebufferHeight;

        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        float panelWidth = std::clamp(windowWidth * 0.35f, 350.0f, 600.0f);
        int graphX = windowWidth > 0 ? static_cast<int>(panelWidth * framebufferWidth / windowWidth) : 0;
        int graphWidth = framebufferWidth - graphX;

        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(0.12f, 0.13f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        RenderUI();

        graphView.Update();
        graphView.Render(graphX, 0, graphWidth, framebufferHeight);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

void Application::RenderUI()
{
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    float panelWidth = std::clamp(windowWidth * 0.35f, 350.0f, 600.0f);

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, static_cast<float>(windowHeight)), ImGuiCond_Always);

    functionEditorPanel.Render(panelWidth, static_cast<float>(windowHeight));
}


void Application::Shutdown()
{
    if (imguiOpenGLInitialized)
        ImGui_ImplOpenGL3_Shutdown();

    if (imguiGlfwInitialized)
        ImGui_ImplGlfw_Shutdown();

    if (imguiContextCreated)
        ImGui::DestroyContext();

    if (graphInitialized)
        graphView.Shutdown();

    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    if (glfwInitialized)
        glfwTerminate();
}
