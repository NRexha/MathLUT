#include <glad/glad.h>
#include "Application.h"

#include <iostream>
#include <vector>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "UI/MathLUTStyle.h"

#pragma region Application Lifecycle

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

#pragma endregion

#pragma region Initialization

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

    if (!functionEditorPanel.Initialize(lutGenerator, graphView, codeFont))
        return false;

    if (!texturePreviewPanel.Initialize(lutGenerator, functionEditorPanel))
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

    ImGuiIO& io = ImGui::GetIO();
    codeFont = io.Fonts->AddFontFromFileTTF("src/Fonts/JetBrainsMono-Regular.ttf", 18.0f);

    if (!codeFont)
        std::cerr << "Failed to load editor font.\n";

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.FontGlobalScale = 1.25f;

    MathLUTStyle::Apply();

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

#pragma endregion

#pragma region Main Loop

void Application::RunLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(0.12f, 0.13f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        RenderUI();
        graphView.Update();
        graphView.RenderTexture(graphWidth, graphHeight);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

#pragma endregion

#pragma region UI Rendering

void Application::RenderUI()
{
    const ImGuiID dockspaceID = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    SetupDockingLayout(dockspaceID);

    functionEditorPanel.Render();
    RenderGraphPanel();
    texturePreviewPanel.Render();
}

void Application::RenderGraphPanel()
{
    if (!ImGui::Begin("Graph"))
    {
        ImGui::End();
        graphWidth = graphHeight = 0;
        graphView.Render(0, 0, 0, 0);
        return;
    }

    const ImVec2 position = ImGui::GetCursorScreenPos();
    const ImVec2 size = ImGui::GetContentRegionAvail();

    if (size.x > 0.0f && size.y > 0.0f)
    {
        int windowWidth, windowHeight;
        int framebufferWidth, framebufferHeight;

        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        if (windowWidth > 0 && windowHeight > 0)
        {
            const float scaleX = static_cast<float>(framebufferWidth) / windowWidth;
            const float scaleY = static_cast<float>(framebufferHeight) / windowHeight;

            graphX = static_cast<int>(position.x * scaleX);
            graphY = framebufferHeight - static_cast<int>((position.y + size.y) * scaleY);
            graphWidth = static_cast<int>(size.x * scaleX);
            graphHeight = static_cast<int>(size.y * scaleY);

            graphView.Render(graphX, graphY, graphWidth, graphHeight);
            ImGui::Image((ImTextureID)(intptr_t)graphView.GetTexture(), size, ImVec2(0, 1), ImVec2(1, 0));
        }
    }
    else
    {
        graphWidth = graphHeight = 0;
        graphView.Render(0, 0, 0, 0);
    }

    ImGui::End();
}



#pragma endregion

#pragma region Docking Layout

void Application::SetupDockingLayout(ImGuiID dockspaceID)
{
    if (dockingLayoutInitialized)
        return;


    ImGui::DockBuilderRemoveNode(dockspaceID);
    ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoWindowMenuButton);
    ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetMainViewport()->Size);

    ImGuiID leftNodeID, graphNodeID;
    ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.50f, &graphNodeID, &leftNodeID);

    ImGuiID editorNodeID, textureNodeID;
    ImGui::DockBuilderSplitNode(leftNodeID, ImGuiDir_Down, 0.2f, &textureNodeID, &editorNodeID);

    ImGui::DockBuilderDockWindow("Function Editor", editorNodeID);
    ImGui::DockBuilderDockWindow("Texture Preview", textureNodeID);
    ImGui::DockBuilderDockWindow("Graph", graphNodeID);

    ImGui::DockBuilderFinish(dockspaceID);
    dockingLayoutInitialized = true;
}

#pragma endregion

#pragma region Shutdown

void Application::Shutdown()
{
    if (window)
        lutGenerator.Shutdown();

    if (imguiOpenGLInitialized)
        ImGui_ImplOpenGL3_Shutdown();

    if (imguiGlfwInitialized)
        ImGui_ImplGlfw_Shutdown();

    if (imguiContextCreated)
        ImGui::DestroyContext();

    if (graphInitialized)
        graphView.Shutdown();

    texturePreviewPanel.Shutdown();

    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    if (glfwInitialized)
        glfwTerminate();
}

#pragma endregion