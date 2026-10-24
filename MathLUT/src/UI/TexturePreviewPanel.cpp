#include "TexturePreviewPanel.h"

#include "FunctionEditorPanel.h"
#include "../LUT/TextureExporter.h"
#include "imgui.h"

#include <utility>
#include <vector>

#pragma region Initialization

bool TexturePreviewPanel::Initialize(LUTGenerator& lutGenerator, FunctionEditorPanel& functionEditorPanel)
{
    this->lutGenerator = &lutGenerator;
    this->functionEditorPanel = &functionEditorPanel;

    return true;
}

#pragma endregion

#pragma region UI Rendering

void TexturePreviewPanel::Render()
{
    if (!ImGui::Begin("Texture Preview"))
    {
        ImGui::End();
        return;
    }

    bool resolutionChanged = false;

    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::BeginCombo("Width", std::to_string(textureWidth).c_str()))
    {
        for (int resolution : { 1, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 })
        {
            const bool selected = textureWidth == resolution;
            if (ImGui::Selectable(std::to_string(resolution).c_str(), selected))
            {
                textureWidth = resolution;
                resolutionChanged = true;
            }

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::BeginCombo("Height", std::to_string(textureHeight).c_str()))
    {
        for (int resolution : { 1, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 })
        {
            const bool selected = textureHeight == resolution;
            if (ImGui::Selectable(std::to_string(resolution).c_str(), selected))
            {
                textureHeight = resolution;
                resolutionChanged = true;
            }

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    const std::string& source = functionEditorPanel->GetActiveSource();

    if (!source.empty() &&
        (source != lastTextureSource ||
            textureWidth != lastTextureWidth ||
            textureHeight != lastTextureHeight ||
            resolutionChanged))
    {
        std::vector<LUTSample> samples;

        if (lutGenerator->GenerateTexture(source, textureWidth, textureHeight, samples))
        {
            if (previewTexture == 0)
                glGenTextures(1, &previewTexture);

            glBindTexture(GL_TEXTURE_2D, previewTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA32F,
                textureWidth, textureHeight, 0,
                GL_RGBA, GL_FLOAT, samples.data());

            glBindTexture(GL_TEXTURE_2D, 0);

            textureSamples = std::move(samples);
            lastTextureSource = source;
            lastTextureWidth = textureWidth;
            lastTextureHeight = textureHeight;
        }
    }

    const float buttonWidth = 100.0f;
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - buttonWidth);

    const bool textureReady = !textureSamples.empty() && lastTextureWidth > 0 && lastTextureHeight > 0;

    if (!textureReady)
        ImGui::BeginDisabled();

    if (ImGui::Button("Export", ImVec2(buttonWidth, 0.0f)))
        TextureExporter::ExportPNG(textureSamples, lastTextureWidth, lastTextureHeight);

    if (!textureReady)
        ImGui::EndDisabled();

    if (previewTexture != 0)
    {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        const float aspectRatio = static_cast<float>(textureWidth) / textureHeight;

        ImVec2 imageSize = available;
        if (imageSize.x / imageSize.y > aspectRatio)
            imageSize.x = imageSize.y * aspectRatio;
        else
            imageSize.y = imageSize.x / aspectRatio;

        ImGui::Image((ImTextureID)(intptr_t)previewTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));
    }
    else
    {
        ImGui::TextUnformatted("Waiting for a valid function...");
    }

    ImGui::End();
}

#pragma endregion

#pragma region Shutdown

void TexturePreviewPanel::Shutdown()
{
    if (previewTexture != 0)
    {
        glDeleteTextures(1, &previewTexture);
        previewTexture = 0;
    }

    textureSamples.clear();
    lastTextureSource.clear();
    lastTextureWidth = 0;
    lastTextureHeight = 0;

    lutGenerator = nullptr;
    functionEditorPanel = nullptr;
}

#pragma endregion