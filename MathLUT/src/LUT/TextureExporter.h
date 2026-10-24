#pragma once

#include "LUTSample.h"

#include <vector>

class TextureExporter
{
public:
    static bool ExportPNG(const std::vector<LUTSample>& samples, int width, int height);
};
