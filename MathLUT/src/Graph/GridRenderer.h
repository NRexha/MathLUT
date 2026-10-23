#pragma once

#include <glad/glad.h>
#include "GraphState.h"

class GridRenderer
{
public:
    bool Initialize();
    void Render(const GraphState& state, int width, int height);
    void Shutdown();

private:
    GLuint programID = 0;
    GLuint vertexArrayID = 0;
    GLint viewportLocation = -1;
    GLint centerLocation = -1;
    GLint zoomLocation = -1;
    GLint gridStepLocation = -1;
};
