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
    startTime = std::chrono::steady_clock::now();
    lastEditTime = startTime;
    lastPreviewUpdate = startTime;

    std::vector<LUTSample> samples;

    if (lutGenerator->GenerateSamples(functionSource, samples, 0.0f))
    {
        activeSource = functionSource;
        graphView->SetSamples(samples);
    }

    return true;
}

void FunctionEditorPanel::Render(float panelWidth, float panelHeight)
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_FirstUseEver);

    ImGui::Begin("Function Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    windowWidth = ImGui::GetWindowWidth();

    editor.Render("##FunctionCode", ImVec2(0.0f, 0.0f));

    ImGui::End();

    UpdateFunction();
}

void FunctionEditorPanel::UpdateFunction()
{
    using Clock = std::chrono::steady_clock;

    const auto now = Clock::now();
    const std::string currentSource = editor.GetText();

    if (currentSource != pendingSource)
    {
        pendingSource = currentSource;
        lastEditTime = now;
    }

    if (pendingSource != lastAttemptedSource && now - lastEditTime >= std::chrono::milliseconds(250))
    {
        lastAttemptedSource = pendingSource;

        const float time = std::chrono::duration<float>(now - startTime).count();
        std::vector<LUTSample> updatedSamples;

        if (lutGenerator->GenerateSamples(pendingSource, updatedSamples, time))
        {
            activeSource = pendingSource;
            graphView->SetSamples(updatedSamples);
            lastPreviewUpdate = now;
        }
    }

    if (currentSource != activeSource || activeSource.empty() || !lutGenerator->UsesTime(activeSource))
        return;

    if (now - lastPreviewUpdate < std::chrono::duration<double>(1.0 / 60.0))
        return;

    const float time = std::chrono::duration<float>(now - startTime).count();
    std::vector<LUTSample> updatedSamples;

    if (lutGenerator->GenerateSamples(activeSource, updatedSamples, time))
    {
        graphView->SetSamples(updatedSamples);
        lastPreviewUpdate = now;
    }
}
