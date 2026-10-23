#include "FunctionEditorPanel.h"

#include "../Core/ShaderSource.h"
#include "imgui.h"

#include <iostream>
#include <vector>

bool FunctionEditorPanel::Initialize(LUTGenerator& generator, GraphView& graph)
{
    lutGenerator = &generator;
    graphView = &graph;

    std::string functionSource;

    if (!ShaderSource::LoadFromFile("src/Functions/TestFunction.mlut", functionSource))
    {
        std::cerr << "Failed to load test function.\n";
        return false;
    }

    editor.SetLanguage(TextEditor::Language::Glsl());
    editor.SetText(functionSource);

    pendingSource = functionSource;
    lastAttemptedSource = functionSource;
    lastEditTime = std::chrono::steady_clock::now();

    std::vector<LUTSample> samples;

    if (lutGenerator->GenerateSamples(functionSource, samples))
        graphView->SetSamples(samples);

    return true;
}

void FunctionEditorPanel::Render(float panelWidth, float panelHeight)
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_Always);

    ImGui::Begin("Function Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    editor.Render("##FunctionCode", ImVec2(0.0f, 0.0f));
    ImGui::End();

    UpdateFunction();
}

void FunctionEditorPanel::UpdateFunction()
{
    std::string currentSource = editor.GetText();

    if (currentSource != pendingSource)
    {
        pendingSource = currentSource;
        lastEditTime = std::chrono::steady_clock::now();
    }

    if (pendingSource != lastAttemptedSource && std::chrono::steady_clock::now() - lastEditTime >= std::chrono::milliseconds(250))
    {
        lastAttemptedSource = pendingSource;
        std::vector<LUTSample> updatedSamples;

        if (lutGenerator->GenerateSamples(pendingSource, updatedSamples))
            graphView->SetSamples(updatedSamples);
    }
}
