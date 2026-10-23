#include "GraphRenderer.h"

GraphRenderer::~GraphRenderer()
{
    Shutdown();
}

bool GraphRenderer::Initialize()
{
    if (!gridRenderer.Initialize())
        return false;

    if (!curveRenderer.Initialize())
    {
        Shutdown();
        return false;
    }

    return true;
}

void GraphRenderer::Render(const GraphState& state, int x, int y, int width, int height)
{
    gridRenderer.Render(state, x, y, width, height);
    curveRenderer.Render(state, x, y, width, height);
}

void GraphRenderer::SetSamples(const std::vector<LUTSample>& samples)
{
    curveRenderer.SetSamples(samples);
}

void GraphRenderer::Shutdown()
{
    curveRenderer.Shutdown();
    gridRenderer.Shutdown();
}

