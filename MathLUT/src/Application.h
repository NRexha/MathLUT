#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Graph/GraphView.h"
#include "LUT/LUTGenerator.h"
#include "UI/FunctionEditorPanel.h"

class Application
{
public:
	int Run();

private:
	bool Initialize();
	bool InitializeWindow();
	bool InitializeImGui();
	void RunLoop();
	void Shutdown();
	void RenderUI();

	GLFWwindow* window = nullptr;
	GraphView graphView;
	LUTGenerator lutGenerator;
	FunctionEditorPanel functionEditorPanel;

	bool glfwInitialized = false;
	bool imguiContextCreated = false;
	bool imguiGlfwInitialized = false;
	bool imguiOpenGLInitialized = false;
	bool graphInitialized = false;
};
