#include "UIStyle.h"

#include "imgui/imgui.h"

namespace nc::editor
{
void SetImGuiStyle()
{
    auto& io = ImGui::GetIO();
    auto& style = ImGui::GetStyle();
    style.WindowMenuButtonPosition = ImGuiDir_None;
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = Color::White;
    colors[ImGuiCol_TextDisabled]          = Color::GrayLightest;
    colors[ImGuiCol_WindowBg]              = Color::GrayDarkest;
    colors[ImGuiCol_ChildBg]               = Color::GrayDark;
    colors[ImGuiCol_PopupBg]               = Color::GrayDark;
    colors[ImGuiCol_Border]                = Color::Black;
    colors[ImGuiCol_BorderShadow]          = Color::AccentTrans;
    colors[ImGuiCol_FrameBg]               = Color::Black;
    colors[ImGuiCol_FrameBgHovered]        = Color::GrayLight;
    colors[ImGuiCol_FrameBgActive]         = Color::TransGrayAlt;
    colors[ImGuiCol_TitleBg]               = Color::Black;
    colors[ImGuiCol_TitleBgActive]         = Color::Black;
    colors[ImGuiCol_TitleBgCollapsed]      = Color::Black;
    colors[ImGuiCol_MenuBarBg]             = Color::GrayDark;
    colors[ImGuiCol_ScrollbarBg]           = Color::TransGrayDark;
    colors[ImGuiCol_ScrollbarGrab]         = Color::Accent;
    colors[ImGuiCol_ScrollbarGrabHovered]  = Color::AccentLight;
    colors[ImGuiCol_ScrollbarGrabActive]   = Color::AccentLight;
    colors[ImGuiCol_CheckMark]             = Color::Accent;
    colors[ImGuiCol_SliderGrab]            = Color::Accent;
    colors[ImGuiCol_SliderGrabActive]      = Color::AccentLight;
    colors[ImGuiCol_Button]                = Color::Black;
    colors[ImGuiCol_ButtonHovered]         = Color::GrayLight;
    colors[ImGuiCol_ButtonActive]          = Color::TransGrayAlt;
    colors[ImGuiCol_Header]                = Color::GrayDarkest;
    colors[ImGuiCol_HeaderHovered]         = Color::Gray;
    colors[ImGuiCol_HeaderActive]          = Color::GrayLight;
    colors[ImGuiCol_Separator]             = Color::TransGray;
    colors[ImGuiCol_SeparatorHovered]      = Color::AccentLight;
    colors[ImGuiCol_SeparatorActive]       = Color::AccentLight;
    colors[ImGuiCol_ResizeGrip]            = Color::Accent;
    colors[ImGuiCol_ResizeGripHovered]     = Color::AccentLight;
    colors[ImGuiCol_ResizeGripActive]      = Color::AccentLight;
    colors[ImGuiCol_Tab]                   = Color::Gray;
    colors[ImGuiCol_TabHovered]            = Color::Black;
    colors[ImGuiCol_TabActive]             = Color::Black;
    colors[ImGuiCol_TabUnfocused]          = Color::Gray;
    colors[ImGuiCol_TabUnfocusedActive]    = Color::Gray;
    colors[ImGuiCol_TextSelectedBg]        = Color::Accent;
    colors[ImGuiCol_DragDropTarget]        = Color::Accent;
    colors[ImGuiCol_NavHighlight]          = Color::Accent;
    colors[ImGuiCol_NavWindowingHighlight] = Color::TransWhite;
    colors[ImGuiCol_NavWindowingDimBg]     = Color::TransGrayLight;
    colors[ImGuiCol_ModalWindowDimBg]      = Color::TransGrayLightAlt;
    colors[ImGuiCol_PlotHistogram]         = Color::AccentDark;
    colors[ImGuiCol_PlotHistogramHovered]  = Color::Accent;
}
}
