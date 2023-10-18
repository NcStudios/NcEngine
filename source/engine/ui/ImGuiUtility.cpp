#include "ncengine/ui/ImGuiUtility.h"

namespace nc::ui
{
void SetDefaultUIStyle()
{
    auto& style = ImGui::GetStyle();
    style.WindowMenuButtonPosition = ImGuiDir_None;
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = default_scheme::White;
    colors[ImGuiCol_TextDisabled]          = default_scheme::GrayLightest;
    colors[ImGuiCol_WindowBg]              = default_scheme::GrayDarkest;
    colors[ImGuiCol_ChildBg]               = default_scheme::GrayDark;
    colors[ImGuiCol_PopupBg]               = default_scheme::GrayDark;
    colors[ImGuiCol_Border]                = default_scheme::Black;
    colors[ImGuiCol_BorderShadow]          = default_scheme::AccentTrans;
    colors[ImGuiCol_FrameBg]               = default_scheme::Black;
    colors[ImGuiCol_FrameBgHovered]        = default_scheme::GrayLight;
    colors[ImGuiCol_FrameBgActive]         = default_scheme::TransGrayAlt;
    colors[ImGuiCol_TitleBg]               = default_scheme::Black;
    colors[ImGuiCol_TitleBgActive]         = default_scheme::Black;
    colors[ImGuiCol_TitleBgCollapsed]      = default_scheme::Black;
    colors[ImGuiCol_MenuBarBg]             = default_scheme::GrayDark;
    colors[ImGuiCol_ScrollbarBg]           = default_scheme::TransGrayDark;
    colors[ImGuiCol_ScrollbarGrab]         = default_scheme::Accent;
    colors[ImGuiCol_ScrollbarGrabHovered]  = default_scheme::AccentLight;
    colors[ImGuiCol_ScrollbarGrabActive]   = default_scheme::AccentLight;
    colors[ImGuiCol_CheckMark]             = default_scheme::Accent;
    colors[ImGuiCol_SliderGrab]            = default_scheme::Accent;
    colors[ImGuiCol_SliderGrabActive]      = default_scheme::AccentLight;
    colors[ImGuiCol_Button]                = default_scheme::Black;
    colors[ImGuiCol_ButtonHovered]         = default_scheme::GrayLight;
    colors[ImGuiCol_ButtonActive]          = default_scheme::TransGrayAlt;
    colors[ImGuiCol_Header]                = default_scheme::GrayDarkest;
    colors[ImGuiCol_HeaderHovered]         = default_scheme::Gray;
    colors[ImGuiCol_HeaderActive]          = default_scheme::GrayLight;
    colors[ImGuiCol_Separator]             = default_scheme::TransGray;
    colors[ImGuiCol_SeparatorHovered]      = default_scheme::AccentLight;
    colors[ImGuiCol_SeparatorActive]       = default_scheme::AccentLight;
    colors[ImGuiCol_ResizeGrip]            = default_scheme::Accent;
    colors[ImGuiCol_ResizeGripHovered]     = default_scheme::AccentLight;
    colors[ImGuiCol_ResizeGripActive]      = default_scheme::AccentLight;
    colors[ImGuiCol_Tab]                   = default_scheme::Gray;
    colors[ImGuiCol_TabHovered]            = default_scheme::Black;
    colors[ImGuiCol_TabActive]             = default_scheme::Black;
    colors[ImGuiCol_TabUnfocused]          = default_scheme::Gray;
    colors[ImGuiCol_TabUnfocusedActive]    = default_scheme::Gray;
    colors[ImGuiCol_TextSelectedBg]        = default_scheme::Accent;
    colors[ImGuiCol_DragDropTarget]        = default_scheme::Accent;
    colors[ImGuiCol_NavHighlight]          = default_scheme::Accent;
    colors[ImGuiCol_NavWindowingHighlight] = default_scheme::TransWhite;
    colors[ImGuiCol_NavWindowingDimBg]     = default_scheme::TransGrayLight;
    colors[ImGuiCol_ModalWindowDimBg]      = default_scheme::TransGrayLightAlt;
    colors[ImGuiCol_PlotHistogram]         = default_scheme::AccentDark;
    colors[ImGuiCol_PlotHistogramHovered]  = default_scheme::Accent;
}
} // namespace nc::ui
