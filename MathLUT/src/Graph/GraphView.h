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
    void Render(int x, int y, int width, int height);
    void Shutdown();
    void SetSamples(const std::vector<LUTSample>& samples);

    const GraphState& GetState() const { return state; }
    int viewportX = 0;
    int viewportY = 0;
    int viewportWidth = 0;
    int viewportHeight = 0;

private:
    static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

    GLFWwindow* window = nullptr;
    GraphState state;
    GraphRenderer renderer;
    bool panning = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

};