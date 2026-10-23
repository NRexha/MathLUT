#include "GraphView.h"

#include <algorithm>
#include <cmath>

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

bool GraphView::Initialize(GLFWwindow* targetWindow)
{
	if (!targetWindow || !renderer.Initialize())
		return false;

	window = targetWindow;
	activeGraphView = this;
	glfwSetScrollCallback(window, ScrollCallback);
	return true;
}

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

void GraphView::Render(int x, int y, int width, int height)
{
	if (!window || width <= 0 || height <= 0)
		return;

	viewportX = x;
	viewportY = y;
	viewportWidth = width;
	viewportHeight = height;

	renderer.Render(state, x, y, width, height);
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

void GraphView::Shutdown()
{
	if (window)
		glfwSetScrollCallback(window, nullptr);

	if (activeGraphView == this)
		activeGraphView = nullptr;

	renderer.Shutdown();
	window = nullptr;
	panning = false;
	viewportX = viewportY = viewportWidth = viewportHeight = 0;
}

void GraphView::SetSamples(const std::vector<LUTSample>& samples)
{
	renderer.SetSamples(samples);
}