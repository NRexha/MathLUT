#define _CRT_SECURE_NO_WARNINGS //bcs use sprintf instead of sprintf_s
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "TexturePreviewPanel.h"

#ifdef APIENTRY
#undef APIENTRY
#endif

#include <Windows.h>
#include <commdlg.h>

#include "FunctionEditorPanel.h"
#include "imgui.h"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>

#define STBIW_WINDOWS_UTF8
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#pragma comment(lib, "Comdlg32.lib")

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
        ExportPNG();

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

//need to move to another file
#pragma region PNG Export

bool TexturePreviewPanel::ExportPNG()
{
    if (textureSamples.empty() || lastTextureWidth <= 0 || lastTextureHeight <= 0)
        return false;

    std::vector<wchar_t> filePath(32768, L'\0');
    std::copy_n(L"MathLUT.png", 12, filePath.begin());

    OPENFILENAMEW dialog = {};
    dialog.lStructSize = sizeof(dialog);
    dialog.lpstrFile = filePath.data();
    dialog.nMaxFile = static_cast<DWORD>(filePath.size());
    dialog.lpstrFilter = L"PNG Image (*.png)\0*.png\0All Files (*.*)\0*.*\0\0";
    dialog.nFilterIndex = 1;
    dialog.lpstrDefExt = L"png";
    dialog.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (!GetSaveFileNameW(&dialog))
        return false;

    const size_t pixelCount = static_cast<size_t>(lastTextureWidth) * lastTextureHeight;
    std::vector<uint8_t> pixels(pixelCount * 4);

    for (int y = 0; y < lastTextureHeight; ++y)
    {
        //reverse the rows so the exported PNG matches the preview orientation
        const int sourceY = lastTextureHeight - 1 - y;

        for (int x = 0; x < lastTextureWidth; ++x)
        {
            const size_t sourceIndex = static_cast<size_t>(sourceY) * lastTextureWidth + x;
            const size_t targetIndex = (static_cast<size_t>(y) * lastTextureWidth + x) * 4;

            const LUTSample& sample = textureSamples[sourceIndex];

            pixels[targetIndex + 0] = static_cast<uint8_t>(std::clamp(sample.r, 0.0f, 1.0f) * 255.0f + 0.5f);
            pixels[targetIndex + 1] = static_cast<uint8_t>(std::clamp(sample.g, 0.0f, 1.0f) * 255.0f + 0.5f);
            pixels[targetIndex + 2] = static_cast<uint8_t>(std::clamp(sample.b, 0.0f, 1.0f) * 255.0f + 0.5f);
            pixels[targetIndex + 3] = static_cast<uint8_t>(std::clamp(sample.a, 0.0f, 1.0f) * 255.0f + 0.5f);
        }
    }

    const int utf8Length = WideCharToMultiByte(CP_UTF8, 0, filePath.data(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Length <= 0)
        return false;

    std::vector<char> utf8Path(static_cast<size_t>(utf8Length));
    if (WideCharToMultiByte(CP_UTF8, 0, filePath.data(), -1, utf8Path.data(), utf8Length, nullptr, nullptr) <= 0)
        return false;

    if (!stbi_write_png(utf8Path.data(), lastTextureWidth, lastTextureHeight, 4, pixels.data(), lastTextureWidth * 4))
    {
        MessageBoxW(nullptr, L"Failed to write the PNG file.", L"MathLUT", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
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