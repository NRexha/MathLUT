#include "GraphView.h"

#include <algorithm>
#include <cmath>

namespace
{
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

bool GraphView::Initialize(GLFWwindow* targetWindow)
{
    if (!targetWindow || !renderer.Initialize())
        return false;

    window = targetWindow;
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, ScrollCallback);
    return true;
}

void GraphView::Update()
{
    if (!window)
        return;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    float x, y;
    int width, height;

    if (!GetFramebufferMousePosition(window, mouseX, mouseY, x, y, width, height))
        return;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        if (panning)
        {
            GraphPoint current = GraphTransform::ScreenToGraph(x, y, static_cast<float>(width), static_cast<float>(height), state);
            GraphPoint previous = GraphTransform::ScreenToGraph(static_cast<float>(lastMouseX), static_cast<float>(lastMouseY), static_cast<float>(width), static_cast<float>(height), state);

            state.centerX -= current.x - previous.x;
            state.centerY -= current.y - previous.y;
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

void GraphView::Render()
{
    if (!window)
        return;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    renderer.Render(state, width, height);
}

void GraphView::ScrollCallback(GLFWwindow* targetWindow, double xOffset, double yOffset)
{
    auto* view = static_cast<GraphView*>(glfwGetWindowUserPointer(targetWindow));

    if (!view)
        return;

    double mouseX, mouseY;
    glfwGetCursorPos(targetWindow, &mouseX, &mouseY);

    float x, y;
    int width, height;

    if (!GetFramebufferMousePosition(targetWindow, mouseX, mouseY, x, y, width, height))
        return;

    GraphPoint anchor = GraphTransform::ScreenToGraph(x, y, static_cast<float>(width), static_cast<float>(height), view->state);

    float oldZoom = view->state.pixelsPerUnit;
    float newZoom = std::clamp(oldZoom * std::pow(1.15f, static_cast<float>(yOffset)), 10.0f, 3000.0f);

    view->state.pixelsPerUnit = newZoom;

    GraphPoint newPosition = GraphTransform::ScreenToGraph(x, y, static_cast<float>(width), static_cast<float>(height), view->state);

    view->state.centerX += anchor.x - newPosition.x;
    view->state.centerY += anchor.y - newPosition.y;
}

void GraphView::Shutdown()
{
    if (window && glfwGetWindowUserPointer(window) == this)
    {
        glfwSetScrollCallback(window, nullptr);
        glfwSetWindowUserPointer(window, nullptr);
    }

    renderer.Shutdown();
    window = nullptr;
    panning = false;
}

void GraphView::SetSamples(const std::vector<LUTSample>& samples)
{
    renderer.SetSamples(samples);
}
