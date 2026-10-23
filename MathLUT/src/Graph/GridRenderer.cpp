#include "GridRenderer.h"
#include "GraphShader.h"

#include <algorithm>
#include <cmath>

namespace
{
    float CalculateGridStep(float pixelsPerUnit)
    {
        float rawStep = 90.0f / pixelsPerUnit;
        float magnitude = std::pow(10.0f, std::floor(std::log10(rawStep)));
        float normalized = rawStep / magnitude;
        float niceStep = normalized < 1.5f ? 1.0f : normalized < 3.5f ? 2.0f : normalized < 7.5f ? 5.0f : 10.0f;
        return niceStep * magnitude;
    }
}

bool GridRenderer::Initialize()
{
    programID = GraphShader::CreateProgram("src/Shaders/Graph.vert", "src/Shaders/Grid.frag");

    if (!programID)
        return false;

    viewportLocation = glGetUniformLocation(programID, "uViewport");
    centerLocation = glGetUniformLocation(programID, "uCenter");
    zoomLocation = glGetUniformLocation(programID, "uPixelsPerUnit");
    gridStepLocation = glGetUniformLocation(programID, "uGridStep");
    viewportOriginLocation = glGetUniformLocation(programID, "uViewportOrigin");
    glGenVertexArrays(1, &vertexArrayID);
    return vertexArrayID != 0;
}

void GridRenderer::Render(const GraphState& state, int x, int y, int width, int height)
{
    if (!programID || !vertexArrayID || width <= 0 || height <= 0)
        return;

    float zoom = std::max(state.pixelsPerUnit, 1.0f);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glUseProgram(programID);
    glViewport(x, y, width, height);
    glUniform2f(viewportLocation, static_cast<float>(width), static_cast<float>(height));
    glUniform2f(viewportOriginLocation, static_cast<float>(x), static_cast<float>(y));
    glUniform2f(centerLocation, state.centerX, state.centerY);
    glUniform1f(zoomLocation, zoom);
    glUniform1f(gridStepLocation, CalculateGridStep(zoom));

    glBindVertexArray(vertexArrayID);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void GridRenderer::Shutdown()
{
    if (vertexArrayID)
    {
        glDeleteVertexArrays(1, &vertexArrayID);
        vertexArrayID = 0;
    }

    if (programID)
    {
        glDeleteProgram(programID);
        programID = 0;
    }
}
