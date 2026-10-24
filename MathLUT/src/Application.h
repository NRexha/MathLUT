#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Graph/GraphView.h"
#include "LUT/LUTGenerator.h"
#include "UI/FunctionEditorPanel.h"
#include "imgui_internal.h"

class Application
{
public:
    //app lifecycle
    int Run();

private:
    bool Initialize();
    bool InitializeWindow();
    bool InitializeImGui();
    void Shutdown();

    void RunLoop();
    void RenderUI();
    void RenderGraphPanel();

    void SetupDockingLayout();
    bool dockingLayoutInitialized = false;

    GLFWwindow* window = nullptr;
    GraphView graphView;
    LUTGenerator lutGenerator;
    FunctionEditorPanel functionEditorPanel;

    bool glfwInitialized = false;
    bool imguiContextCreated = false;
    bool imguiGlfwInitialized = false;
    bool imguiOpenGLInitialized = false;
    bool graphInitialized = false;

    int graphX = 0;
    int graphY = 0;
    int graphWidth = 0;
    int graphHeight = 0;

    ImFont* codeFont = nullptr;
};