#pragma once

#include "../LUT/LUTGenerator.h"

#include <glad/glad.h>

#include <string>
#include <vector>

class FunctionEditorPanel;

class TexturePreviewPanel
{
public:
    //initialization and UI
    bool Initialize(LUTGenerator& lutGenerator, FunctionEditorPanel& functionEditorPanel);
    void Render();
    void Shutdown();

private:
    //PNG export
    bool ExportPNG();

    LUTGenerator* lutGenerator = nullptr;
    FunctionEditorPanel* functionEditorPanel = nullptr;

    GLuint previewTexture = 0;
    int textureWidth = 1024;
    int textureHeight = 64;

    std::string lastTextureSource;
    int lastTextureWidth = 0;
    int lastTextureHeight = 0;

    std::vector<LUTSample> textureSamples;
};