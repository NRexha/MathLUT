#include "MathLUTStyle.h"
#include "imgui.h"

void MathLUTStyle::Apply()
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(8.0f, 5.0f);
    style.CellPadding = ImVec2(6.0f, 5.0f);
    style.ItemSpacing = ImVec2(8.0f, 7.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 5.0f);
    style.IndentSpacing = 16.0f;
    style.ScrollbarSize = 12.0f;
    style.GrabMinSize = 10.0f;

    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;

    style.WindowRounding = 0.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;

    ImVec4* colors = style.Colors;

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.86f, 0.87f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.37f, 0.39f, 0.44f, 1.00f);

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.094f, 0.102f, 0.125f, 1.00f); // #181A20
    colors[ImGuiCol_ChildBg] = ImVec4(0.094f, 0.102f, 0.125f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.105f, 0.114f, 0.137f, 0.98f);

    // Borders and separators
    colors[ImGuiCol_Border] = ImVec4(0.176f, 0.192f, 0.231f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.176f, 0.192f, 0.231f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30f, 0.48f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.35f, 0.57f, 0.84f, 1.00f);

    // Input fields
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.19f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.19f, 0.23f, 0.30f, 1.00f);

    // Window title bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.075f, 0.082f, 0.100f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.105f, 0.125f, 0.160f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.075f, 0.082f, 0.100f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.094f, 0.102f, 0.125f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.16f, 0.19f, 0.24f, 1.00f);
    colors[ImGuiCol_TabSelected] = ImVec4(0.125f, 0.133f, 0.157f, 1.00f);
    colors[ImGuiCol_TabDimmed] = ImVec4(0.075f, 0.082f, 0.100f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.105f, 0.114f, 0.137f, 1.00f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.27f, 0.32f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.32f, 0.37f, 0.44f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.38f, 0.45f, 0.55f, 1.00f);

    // Checkboxes and sliders
    colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.65f, 0.92f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.57f, 0.84f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.48f, 0.68f, 0.94f, 1.00f);

    // Buttons
    colors[ImGuiCol_Button] = ImVec4(0.15f, 0.19f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.28f, 0.38f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.35f, 0.48f, 1.00f);

    // Headers and selectable items
    colors[ImGuiCol_Header] = ImVec4(0.15f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.29f, 0.40f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.23f, 0.34f, 0.47f, 1.00f);

    // Docking
    colors[ImGuiCol_DockingPreview] = ImVec4(0.30f, 0.52f, 0.78f, 0.45f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.075f, 0.082f, 0.100f, 1.00f);

    // Navigation
    colors[ImGuiCol_NavCursor] = ImVec4(0.40f, 0.65f, 0.92f, 0.80f);

    style.WindowMenuButtonPosition = ImGuiDir_None;
}
