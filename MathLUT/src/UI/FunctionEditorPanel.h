#pragma once

#include "../LUT/LUTGenerator.h"
#include "../Graph/GraphView.h"
#include "TextEditor.h"

#include <chrono>
#include <string>

class FunctionEditorPanel
{
public:
    bool Initialize(LUTGenerator& lutGenerator, GraphView& graphView);
    void Render(float panelWidth, float panelHeight);
    float GetWidth() const { return windowWidth;}

private:
    void UpdateFunction();

    TextEditor editor;
    LUTGenerator* lutGenerator = nullptr;
    GraphView* graphView = nullptr;

    std::string pendingSource;
    std::string lastAttemptedSource;
    std::string activeSource;

    std::chrono::steady_clock::time_point lastEditTime;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastPreviewUpdate;
    float windowWidth = 450.0f;
};
