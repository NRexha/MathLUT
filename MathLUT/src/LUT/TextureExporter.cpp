#define _CRT_SECURE_NO_WARNINGS //bcs use sprintf instead of sprintf_s
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "TextureExporter.h"

#include <Windows.h>
#include <commdlg.h>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <vector>

#define STBIW_WINDOWS_UTF8
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#pragma comment(lib, "Comdlg32.lib")

#pragma region PNG Export

bool TextureExporter::ExportPNG(const std::vector<LUTSample>& samples, int width, int height)
{
    if (samples.empty() || width <= 0 || height <= 0 ||
        samples.size() != static_cast<size_t>(width) * height)
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

    const size_t pixelCount = static_cast<size_t>(width) * height;
    std::vector<uint8_t> pixels(pixelCount * 4);

    for (int y = 0; y < height; ++y)
    {
        //Reverse the rows so the exported PNG matches the preview orientation.
        const int sourceY = height - 1 - y;

        for (int x = 0; x < width; ++x)
        {
            const size_t sourceIndex = static_cast<size_t>(sourceY) * width + x;
            const size_t targetIndex = (static_cast<size_t>(y) * width + x) * 4;

            const LUTSample& sample = samples[sourceIndex];

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

    if (!stbi_write_png(utf8Path.data(), width, height, 4, pixels.data(), width * 4))
    {
        MessageBoxW(nullptr, L"Failed to write the PNG file.", L"MathLUT", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

#pragma endregion
