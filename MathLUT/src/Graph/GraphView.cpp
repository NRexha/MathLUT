#include "GraphView.h"

#include <algorithm>
#include <cmath>

#pragma region Helpers

namespace
{
    GraphView* activeGraphView = nullptr;

    bool GetFramebufferMousePosition(GLFWwindow* window, double mouseX, double mouseY, float& x, float& y, int& width, int& height)
    {
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwGetFramebufferSize(window, &width, &height);

        if (windowWidth <= 0 || windowHeight <= 0 || width <= 0 || height <= 0)
            return false;

        x = static_cast<float>(mouseX) * width / windowWidth;
        y = static_cast<float>(mouseY) * height / windowHeight;
        return true;
    }
}

#pragma endregion

#pragma region Lifecycle

bool GraphView::Initialize(GLFWwindow* targetWindow)
{
    if (!targetWindow || !renderer.Initialize())
        return false;

    window = targetWindow;
    activeGraphView = this;
    glfwSetScrollCallback(window, ScrollCallback);
    return true;
}

void GraphView::Shutdown()
{
    if (window)
        glfwSetScrollCallback(window, nullptr);

    if (activeGraphView == this)
        activeGraphView = nullptr;

    renderer.Shutdown();

    if (graphTexture)
        glDeleteTextures(1, &graphTexture);

    if (framebuffer)
        glDeleteFramebuffers(1, &framebuffer);

    framebuffer = graphTexture = 0;
    textureWidth = textureHeight = 0;
    window = nullptr;
    panning = false;
    viewportX = viewportY = viewportWidth = viewportHeight = 0;
}

#pragma endregion

#pragma region Interaction

void GraphView::Update()
{
    if (!window || viewportWidth <= 0 || viewportHeight <= 0)
        return;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    float x, y;
    int width, height;

    if (!GetFramebufferMousePosition(window, mouseX, mouseY, x, y, width, height))
        return;

    x -= static_cast<float>(viewportX);
    y = static_cast<float>(height) - y - static_cast<float>(viewportY);

    if (x < 0.0f || x >= viewportWidth || y < 0.0f || y >= viewportHeight)
    {
        panning = false;
        return;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        if (panning)
        {
            GraphPoint current = GraphTransform::ScreenToGraph(x, y, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), state);
            GraphPoint previous = GraphTransform::ScreenToGraph(static_cast<float>(lastMouseX), static_cast<float>(lastMouseY), static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), state);

            state.centerX -= current.x - previous.x;
            state.centerY += current.y - previous.y;
        }

        panning = true;
    }
    else
    {
        panning = false;
    }

    lastMouseX = x;
    lastMouseY = y;
}

void GraphView::ScrollCallback(GLFWwindow* targetWindow, double xOffset, double yOffset)
{
    auto* view = activeGraphView;

    if (!view || view->window != targetWindow || view->viewportWidth <= 0 || view->viewportHeight <= 0)
        return;

    double mouseX, mouseY;
    glfwGetCursorPos(targetWindow, &mouseX, &mouseY);

    float x, y;
    int width, height;

    if (!GetFramebufferMousePosition(targetWindow, mouseX, mouseY, x, y, width, height))
        return;

    x -= static_cast<float>(view->viewportX);
    y = static_cast<float>(height) - y - static_cast<float>(view->viewportY);

    if (x < 0.0f || x >= view->viewportWidth || y < 0.0f || y >= view->viewportHeight)
        return;

    GraphPoint anchor = GraphTransform::ScreenToGraph(x, y, static_cast<float>(view->viewportWidth), static_cast<float>(view->viewportHeight), view->state);

    float oldZoom = view->state.pixelsPerUnit;
    float newZoom = std::clamp(oldZoom * std::pow(1.15f, static_cast<float>(yOffset)), 10.0f, 3000.0f);

    view->state.pixelsPerUnit = newZoom;

    GraphPoint newPosition = GraphTransform::ScreenToGraph(x, y, static_cast<float>(view->viewportWidth), static_cast<float>(view->viewportHeight), view->state);

    view->state.centerX += anchor.x - newPosition.x;
    view->state.centerY += anchor.y - newPosition.y;
}

#pragma endregion

#pragma region Rendering

void GraphView::Render(int x, int y, int width, int height)
{
    viewportX = x;
    viewportY = y;
    viewportWidth = width;
    viewportHeight = height;
}

void GraphView::RenderTexture(int width, int height)
{
    if (!window || width <= 0 || height <= 0 || !ResizeFramebuffer(width, height))
        return;

    GLint previousFramebuffer;
    GLint previousViewport[4];

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glGetIntegerv(GL_VIEWPORT, previousViewport);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
    glClearColor(0.12f, 0.13f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderer.Render(state, 0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
    glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
}

bool GraphView::ResizeFramebuffer(int width, int height)
{
    if (width <= 0 || height <= 0)
        return false;

    if (framebuffer && textureWidth == width && textureHeight == height)
        return true;

    if (!framebuffer)
    {
        glGenFramebuffers(1, &framebuffer);
        glGenTextures(1, &graphTexture);
    }

    glBindTexture(GL_TEXTURE_2D, graphTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, graphTexture, 0);

    const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (!complete)
    {
        textureWidth = textureHeight = 0;
        return false;
    }

    textureWidth = width;
    textureHeight = height;
    return true;
}

#pragma endregion

#pragma region Samples

void GraphView::SetSamples(const std::vector<LUTSample>& samples)
{
    renderer.SetSamples(samples);
}

#pragma endregion
