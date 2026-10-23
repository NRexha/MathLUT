#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "GraphState.h"
#include "GraphRenderer.h"
#include "../LUT/LUTSample.h"
#include "GraphTransform.h"
#include <vector>

class GraphView
{
public:
    bool Initialize(GLFWwindow* window);
    void Update();
    void Render();
    void Shutdown();
    void SetSamples(const std::vector<LUTSample>& samples);

    const GraphState& GetState() const { return state; }

private:
    static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

    GLFWwindow* window = nullptr;
    GraphState state;
    GraphRenderer renderer;
    bool panning = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
};