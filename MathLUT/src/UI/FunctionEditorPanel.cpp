#include "FunctionEditorPanel.h"
#include "../Core/ShaderSource.h"
#include "../Language/BuiltinRegistry.h"

#include "imgui.h"

#include <algorithm>
#include <iostream>
#include <vector>

#pragma region Constants

namespace
{
    constexpr ImU32 EditorBackground = IM_COL32(24, 26, 32, 255);
}

#pragma endregion

#pragma region Initialization

bool FunctionEditorPanel::Initialize(LUTGenerator& generator, GraphView& graph, ImFont* font)
{
    lutGenerator = &generator;
    graphView = &graph;
    codeFont = font;

    std::string functionSource;

    if (!ShaderSource::LoadFromFile("src/Functions/TestFunction.mlut", functionSource))
    {
        std::cerr << "Failed to load test function.\n";
        return false;
    }

    //editor Styling
    editor.SetLanguage(TextEditor::Language::Glsl());

    TextEditor::Palette palette = TextEditor::GetDarkPalette();

    palette[static_cast<size_t>(TextEditor::Color::background)] = EditorBackground;
    palette[static_cast<size_t>(TextEditor::Color::text)] = IM_COL32(220, 223, 230, 255);
    palette[static_cast<size_t>(TextEditor::Color::keyword)] = IM_COL32(198, 120, 221, 255);
    palette[static_cast<size_t>(TextEditor::Color::declaration)] = IM_COL32(97, 175, 239, 255);
    palette[static_cast<size_t>(TextEditor::Color::number)] = IM_COL32(209, 154, 102, 255);
    palette[static_cast<size_t>(TextEditor::Color::string)] = IM_COL32(152, 195, 121, 255);
    palette[static_cast<size_t>(TextEditor::Color::comment)] = IM_COL32(110, 120, 135, 255);
    palette[static_cast<size_t>(TextEditor::Color::identifier)] = IM_COL32(220, 223, 230, 255);
    palette[static_cast<size_t>(TextEditor::Color::knownIdentifier)] = IM_COL32(97, 175, 239, 255);
    palette[static_cast<size_t>(TextEditor::Color::cursor)] = IM_COL32(220, 223, 230, 255);
    palette[static_cast<size_t>(TextEditor::Color::selection)] = IM_COL32(65, 85, 115, 180);
    palette[static_cast<size_t>(TextEditor::Color::lineNumber)] = IM_COL32(95, 101, 113, 255);
    palette[static_cast<size_t>(TextEditor::Color::currentLineNumber)] = IM_COL32(220, 223, 230, 255);
    palette[static_cast<size_t>(TextEditor::Color::currentLineHighlight)] = IM_COL32(35, 38, 46, 255);
    palette[static_cast<size_t>(TextEditor::Color::currentLineHighlightBorder)] = IM_COL32(45, 49, 59, 255);

    editor.SetPalette(palette);


    //autocomplete

    autoCompleteConfig.triggerOnTyping = true;
    autoCompleteConfig.triggerOnShortcut = true;
    autoCompleteConfig.triggerInComments = false;
    autoCompleteConfig.triggerInStrings = false;
    autoCompleteConfig.triggerDelay = std::chrono::milliseconds(150);
    autoCompleteConfig.suggestionWidth = 24;

    autoCompleteConfig.callback = [](TextEditor::AutoCompleteState& state)
        {
            state.suggestions.clear();

            const std::string& term = state.searchTerm;

            static const std::vector<std::string> functions =
            {
                "abs", "acos", "asin", "atan", "ceil", "clamp", "cos", "degrees", "distance", "dot", "exp", "exp2", "floor", "fract", "length", "log", "log2",
                "max", "min", "mix", "mod", "normalize", "pow", "radians", "reflect", "refract", "sign", "sin", "smoothstep", "sqrt", "step", "tan"
            };

            for (const BuiltinVariable& builtin : BuiltinRegistry::GetAll())
            {
                if (term.empty() || builtin.name.compare(0, term.size(), term) == 0)
                    state.suggestions.push_back(builtin.name);
            }

            for (const std::string& function : functions)
            {
                if (term.empty() || function.compare(0, term.size(), term) == 0)
                    state.suggestions.push_back(function);
            }

            std::sort(state.suggestions.begin(), state.suggestions.end());
        };

    editor.SetAutoCompleteConfig(&autoCompleteConfig);



    //source and preview

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

#pragma endregion

#pragma region Rendering

void FunctionEditorPanel::Render()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, EditorBackground);
    ImGui::Begin("Function Editor");

    if (codeFont)
        ImGui::PushFont(codeFont);

    editor.Render("##FunctionCode", ImGui::GetContentRegionAvail());

    if (codeFont)
        ImGui::PopFont();

    ImGui::End();
    ImGui::PopStyleColor();

    UpdateFunction();
}

#pragma endregion

#pragma region Preview Updates

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

#pragma region Debounced Compilation

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

#pragma endregion

#pragma region Animated Preview

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

#pragma endregion
}

#pragma endregion
