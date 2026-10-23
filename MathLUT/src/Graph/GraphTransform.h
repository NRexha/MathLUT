#pragma once

#include "GraphState.h"

struct GraphPoint
{
    float x;
    float y;
};

class GraphTransform
{
public:
    static GraphPoint ScreenToGraph(float x, float y, float width, float height, const GraphState& state)
    {
        return 
        {
            state.centerX + (x - width * 0.5f) / state.pixelsPerUnit, state.centerY + (height * 0.5f - y) / state.pixelsPerUnit
        };
    }

    static GraphPoint GraphToScreen(float x, float y, float width, float height, const GraphState& state)
    {
        return 
        {
            (x - state.centerX) * state.pixelsPerUnit + width * 0.5f,height * 0.5f - (y - state.centerY) * state.pixelsPerUnit
        };
    }
};
