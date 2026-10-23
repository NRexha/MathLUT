#pragma once

#include "GraphState.h"
#include "GridRenderer.h"
#include "CurveRenderer.h"
#include "../LUT/LUTSample.h"
#include <vector>

class GraphRenderer
{
public:
    GraphRenderer() = default;
    ~GraphRenderer();

    GraphRenderer(const GraphRenderer&) = delete;
    GraphRenderer& operator=(const GraphRenderer&) = delete;

    bool Initialize();
    void Render(const GraphState& state, int x, int y, int width, int height);
    void Shutdown();
    void SetSamples(const std::vector<LUTSample>& samples);

private:
    GridRenderer gridRenderer;
    CurveRenderer curveRenderer;
};
