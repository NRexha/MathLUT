#include "CurveRenderer.h"
#include "GraphShader.h"

#include <algorithm>

#pragma region Initialization

bool CurveRenderer::Initialize()
{
    programID = GraphShader::CreateProgram("src/Shaders/Graph.vert", "src/Shaders/Curve.frag");

    if (!programID)
        return false;

    viewportLocation = glGetUniformLocation(programID, "uViewport");
    centerLocation = glGetUniformLocation(programID, "uCenter");
    zoomLocation = glGetUniformLocation(programID, "uPixelsPerUnit");
    channelsLocation = glGetUniformLocation(programID, "uVisibleChannels");
    samplesLocation = glGetUniformLocation(programID, "uSamples");
    viewportOriginLocation = glGetUniformLocation(programID, "uViewportOrigin");

    glGenVertexArrays(1, &vertexArrayID);

    return vertexArrayID != 0;
}

#pragma endregion

#pragma region Rendering

void CurveRenderer::Render(const GraphState& state, int x, int y, int width, int height)
{
    if (!programID || !sampleTextureID || width <= 0 || height <= 0)
        return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(programID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, sampleTextureID);

    glUniform1i(samplesLocation, 0);
    glViewport(x, y, width, height);
    glUniform2f(viewportLocation, static_cast<float>(width), static_cast<float>(height));
    glUniform2f(viewportOriginLocation, static_cast<float>(x), static_cast<float>(y));
    glUniform2f(centerLocation, state.centerX, state.centerY);
    glUniform1f(zoomLocation, std::max(state.pixelsPerUnit, 1.0f));
    glUniform4f(channelsLocation, state.showRed ? 1.0f : 0.0f, state.showGreen ? 1.0f : 0.0f, state.showBlue ? 1.0f : 0.0f, state.showAlpha ? 1.0f : 0.0f);

    glBindVertexArray(vertexArrayID);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_1D, 0);
    glDisable(GL_BLEND);
}

#pragma endregion

#pragma region Sample Management

void CurveRenderer::SetSamples(const std::vector<LUTSample>& samples)
{
    if (samples.empty())
        return;

    if (!sampleTextureID)
        glGenTextures(1, &sampleTextureID);

    glBindTexture(GL_TEXTURE_1D, sampleTextureID);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, static_cast<GLsizei>(samples.size()), 0, GL_RGBA, GL_FLOAT, samples.data());
    glBindTexture(GL_TEXTURE_1D, 0);
}

#pragma endregion

#pragma region Cleanup

void CurveRenderer::Shutdown()
{
    if (sampleTextureID)
    {
        glDeleteTextures(1, &sampleTextureID);
        sampleTextureID = 0;
    }

    if (programID)
    {
        glDeleteProgram(programID);
        programID = 0;
    }

    if (vertexArrayID)
    {
        glDeleteVertexArrays(1, &vertexArrayID);
        vertexArrayID = 0;
    }
}

#pragma endregion