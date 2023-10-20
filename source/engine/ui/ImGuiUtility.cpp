#include "ncengine/ui/ImGuiUtility.h"

namespace nc::ui
{
void SetDefaultUIStyle()
{
    auto& style = ImGui::GetStyle();
    style.WindowMenuButtonPosition = ImGuiDir_None;
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = default_scheme::Text;
    colors[ImGuiCol_TextDisabled]          = default_scheme::Disabled;
    colors[ImGuiCol_WindowBg]              = default_scheme::PrimaryBG;
    colors[ImGuiCol_ChildBg]               = default_scheme::SecondaryBG;
    colors[ImGuiCol_PopupBg]               = default_scheme::SecondaryBG;
    colors[ImGuiCol_Border]                = default_scheme::Border;
    colors[ImGuiCol_BorderShadow]          = default_scheme::AccentTrans;
    colors[ImGuiCol_FrameBg]               = default_scheme::Title;
    colors[ImGuiCol_FrameBgHovered]        = default_scheme::Hovered;
    colors[ImGuiCol_FrameBgActive]         = default_scheme::Active;
    colors[ImGuiCol_TitleBg]               = default_scheme::Title;
    colors[ImGuiCol_TitleBgActive]         = default_scheme::Title;
    colors[ImGuiCol_TitleBgCollapsed]      = default_scheme::Title;
    colors[ImGuiCol_MenuBarBg]             = default_scheme::SecondaryBG;
    colors[ImGuiCol_ScrollbarBg]           = default_scheme::ScrollBarBG;
    colors[ImGuiCol_ScrollbarGrab]         = default_scheme::Accent;
    colors[ImGuiCol_ScrollbarGrabHovered]  = default_scheme::AccentActive;
    colors[ImGuiCol_ScrollbarGrabActive]   = default_scheme::AccentActive;
    colors[ImGuiCol_CheckMark]             = default_scheme::Accent;
    colors[ImGuiCol_SliderGrab]            = default_scheme::Accent;
    colors[ImGuiCol_SliderGrabActive]      = default_scheme::AccentActive;
    colors[ImGuiCol_Button]                = default_scheme::Button;
    colors[ImGuiCol_ButtonHovered]         = default_scheme::Hovered;
    colors[ImGuiCol_ButtonActive]          = default_scheme::Active;
    colors[ImGuiCol_Header]                = default_scheme::PrimaryBG;
    colors[ImGuiCol_HeaderHovered]         = default_scheme::Tabs;
    colors[ImGuiCol_HeaderActive]          = default_scheme::Hovered;
    colors[ImGuiCol_Separator]             = default_scheme::Separator;
    colors[ImGuiCol_SeparatorHovered]      = default_scheme::AccentActive;
    colors[ImGuiCol_SeparatorActive]       = default_scheme::AccentActive;
    colors[ImGuiCol_ResizeGrip]            = default_scheme::Accent;
    colors[ImGuiCol_ResizeGripHovered]     = default_scheme::AccentActive;
    colors[ImGuiCol_ResizeGripActive]      = default_scheme::AccentActive;
    colors[ImGuiCol_Tab]                   = default_scheme::Tabs;
    colors[ImGuiCol_TabHovered]            = default_scheme::Title;
    colors[ImGuiCol_TabActive]             = default_scheme::Title;
    colors[ImGuiCol_TabUnfocused]          = default_scheme::Tabs;
    colors[ImGuiCol_TabUnfocusedActive]    = default_scheme::Tabs;
    colors[ImGuiCol_TextSelectedBg]        = default_scheme::Accent;
    colors[ImGuiCol_DragDropTarget]        = default_scheme::Accent;
    colors[ImGuiCol_NavHighlight]          = default_scheme::Accent;
    colors[ImGuiCol_NavWindowingHighlight] = default_scheme::WindowHighlight;
    colors[ImGuiCol_NavWindowingDimBg]     = default_scheme::DimBg;
    colors[ImGuiCol_ModalWindowDimBg]      = default_scheme::DimBg;
    colors[ImGuiCol_PlotHistogram]         = default_scheme::AccentDim;
    colors[ImGuiCol_PlotHistogramHovered]  = default_scheme::Accent;
}
} // namespace nc::ui
