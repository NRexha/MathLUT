#pragma once

#include "../LUT/LUTGenerator.h"
#include "../Graph/GraphView.h"
#include "TextEditor.h"

#include <chrono>
#include <string>

class FunctionEditorPanel
{
public:
    //initialization and UI
    bool Initialize(LUTGenerator& lutGenerator, GraphView& graphView, ImFont* codeFont);
    void Render();

private:
    void UpdateFunction();

    TextEditor editor;
    LUTGenerator* lutGenerator = nullptr;
    GraphView* graphView = nullptr;
    ImFont* codeFont = nullptr;

    std::string pendingSource;
    std::string lastAttemptedSource;
    std::string activeSource;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastEditTime;
    std::chrono::steady_clock::time_point lastPreviewUpdate;

    TextEditor::AutoCompleteConfig autoCompleteConfig;
};