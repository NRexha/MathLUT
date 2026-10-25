#include "ConsolePanel.h"

#include "imgui.h"

#pragma region Message Management

void ConsolePanel::Log(ConsoleMessageType type, const std::string& message)
{
    messages.push_back({ type, message });
}

void ConsolePanel::Clear()
{
    messages.clear();
}

#pragma endregion


void ConsolePanel::Render()
{
    if (!ImGui::Begin("Console"))
    {
        ImGui::End();
        return;
    }


    const bool wasAtBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();

    if (ImGui::BeginChild("ConsoleMessages", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        for (const ConsoleMessage& message : messages)
        {
            ImVec4 color;

            switch (message.type)
            {
            case ConsoleMessageType::Success:
                color = ImVec4(0.40f, 0.80f, 0.48f, 1.0f);
                break;
            case ConsoleMessageType::Warning:
                color = ImVec4(0.95f, 0.75f, 0.35f, 1.0f);
                break;
            case ConsoleMessageType::Error:
                color = ImVec4(1.0f, 0.38f, 0.38f, 1.0f);
                break;
            default:
                color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
                break;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextWrapped("%s", message.message.c_str());
            ImGui::PopStyleColor();
        }

        if (wasAtBottom)
            ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}

