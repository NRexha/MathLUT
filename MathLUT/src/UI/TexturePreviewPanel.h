#pragma once

#include "../LUT/LUTGenerator.h"

#include <glad/glad.h>

#include <string>
#include <vector>

class FunctionEditorPanel;
class ConsolePanel;

class TexturePreviewPanel
{
public:
    //initialization and UI
    bool Initialize(LUTGenerator& lutGenerator, FunctionEditorPanel& functionEditorPanel, ConsolePanel& consolePanel);
    void Render();
    void Shutdown();

private:
    LUTGenerator* lutGenerator = nullptr;
    FunctionEditorPanel* functionEditorPanel = nullptr;
    ConsolePanel* consolePanel = nullptr;

    GLuint previewTexture = 0;
    int textureWidth = 64;
    int textureHeight = 4;

    std::string lastTextureSource;
    int lastTextureWidth = 0;
    int lastTextureHeight = 0;

    //Track attempts to avoid repeating the same error every frame.
    std::string lastAttemptedSource;
    int lastAttemptedWidth = 0;
    int lastAttemptedHeight = 0;

    std::vector<LUTSample> textureSamples;
};
