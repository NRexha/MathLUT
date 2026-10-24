#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>

#include "GraphState.h"
#include "GraphRenderer.h"
#include "GraphTransform.h"
#include "../LUT/LUTSample.h"

class GraphView
{
public:
    //lifecycle
    bool Initialize(GLFWwindow* window);
    void Update();
    void Shutdown();

    //rendering
    void Render(int x, int y, int width, int height);
    void RenderTexture(int width, int height);
    GLuint GetTexture() const { return graphTexture; }

    //graph data
    void SetSamples(const std::vector<LUTSample>& samples);
    const GraphState& GetState() const { return state; }

    //viewport in framebuffer coordinates
    int viewportX = 0;
    int viewportY = 0;
    int viewportWidth = 0;
    int viewportHeight = 0;

private:
    static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    bool ResizeFramebuffer(int width, int height);

    GLFWwindow* window = nullptr;
    GraphState state;
    GraphRenderer renderer;

    bool panning = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    GLuint framebuffer = 0;
    GLuint graphTexture = 0;
    int textureWidth = 0;
    int textureHeight = 0;
};