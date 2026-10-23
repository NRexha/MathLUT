#pragma once

#include <glad/glad.h>
#include "GraphState.h"
#include "../LUT/LUTSample.h"
#include <vector>

class CurveRenderer
{
public:
    bool Initialize();
    void Render(const GraphState& state, int x, int y, int width, int height);
    void SetSamples(const std::vector<LUTSample>& samples);
    void Shutdown();

private:
    GLuint programID = 0;
    GLuint vertexArrayID = 0;
    GLuint sampleTextureID = 0;
    GLint viewportLocation = -1;
    GLint centerLocation = -1;
    GLint zoomLocation = -1;
    GLint channelsLocation = -1;
    GLint samplesLocation = -1;
    GLint viewportOriginLocation = -1;
};
