#pragma once

#include "imgui/imgui.h"

namespace nc::ui
{
    const ImVec4 COLOR_WHITE                (1.000f, 1.000f, 1.000f, 1.000f);
    const ImVec4 COLOR_TRANS_WHITE          (1.000f, 1.000f, 1.000f, 0.700f);
    const ImVec4 COLOR_TRANS                (0.000f, 0.000f, 0.000f, 0.000f);
    const ImVec4 COLOR_TRANS_GRAY_DARK      (0.020f, 0.020f, 0.020f, 0.530f);
    const ImVec4 COLOR_TRANS_GRAY           (0.430f, 0.430f, 0.500f, 0.500f);
    const ImVec4 COLOR_TRANS_GRAY_ALT       (0.670f, 0.670f, 0.670f, 0.390f);
    const ImVec4 COLOR_TRANS_GRAY_LIGHT     (0.800f, 0.800f, 0.800f, 0.200f);
    const ImVec4 COLOR_TRANS_GRAY_LIGHT_ALT (0.800f, 0.800f, 0.800f, 0.350f);
    const ImVec4 COLOR_GRAY_DARKEST         (0.130f, 0.140f, 0.150f, 1.000f);
    const ImVec4 COLOR_GRAY_DARK            (0.200f, 0.200f, 0.200f, 1.000f);
    const ImVec4 COLOR_GRAY                 (0.250f, 0.250f, 0.250f, 1.000f);
    const ImVec4 COLOR_GRAY_LIGHT           (0.380f, 0.380f, 0.380f, 1.000f);
    const ImVec4 COLOR_GRAY_LIGHTEST        (0.500f, 0.500f, 0.500f, 1.000f);
    const ImVec4 COLOR_ACCENT_DARK          (0.333f, 0.157f, 0.000f, 1.000f);
    const ImVec4 COLOR_ACCENT               (0.502f, 0.278f, 0.082f, 1.000f);
    const ImVec4 COLOR_ACCENT_LIGHT         (0.831f, 0.612f, 0.416f, 1.000f);

    void SetImGuiStyle()
    {
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigDockingWithShift = false;
        auto& style = ImGui::GetStyle();
        style.WindowMenuButtonPosition = ImGuiDir_None;
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_Text]                  = COLOR_WHITE;
        colors[ImGuiCol_TextDisabled]          = COLOR_GRAY_LIGHTEST;
        colors[ImGuiCol_WindowBg]              = COLOR_GRAY_DARKEST;
        colors[ImGuiCol_ChildBg]               = COLOR_GRAY_DARKEST;
        colors[ImGuiCol_PopupBg]               = COLOR_GRAY_DARKEST;
        colors[ImGuiCol_Border]                = COLOR_TRANS_GRAY;
        colors[ImGuiCol_BorderShadow]          = COLOR_TRANS;
        colors[ImGuiCol_FrameBg]               = COLOR_GRAY;
        colors[ImGuiCol_FrameBgHovered]        = COLOR_GRAY_LIGHT;
        colors[ImGuiCol_FrameBgActive]         = COLOR_TRANS_GRAY_ALT;
        colors[ImGuiCol_TitleBg]               = COLOR_ACCENT;
        colors[ImGuiCol_TitleBgActive]         = COLOR_ACCENT;
        colors[ImGuiCol_TitleBgCollapsed]      = COLOR_ACCENT;
        colors[ImGuiCol_MenuBarBg]             = COLOR_ACCENT;
        colors[ImGuiCol_ScrollbarBg]           = COLOR_TRANS_GRAY_DARK;
        colors[ImGuiCol_ScrollbarGrab]         = COLOR_ACCENT;
        colors[ImGuiCol_ScrollbarGrabHovered]  = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_ScrollbarGrabActive]   = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_CheckMark]             = COLOR_ACCENT;
        colors[ImGuiCol_SliderGrab]            = COLOR_ACCENT;
        colors[ImGuiCol_SliderGrabActive]      = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_Button]                = COLOR_GRAY;
        colors[ImGuiCol_ButtonHovered]         = COLOR_GRAY_LIGHT;
        colors[ImGuiCol_ButtonActive]          = COLOR_TRANS_GRAY_ALT;
        colors[ImGuiCol_Header]                = COLOR_GRAY;
        colors[ImGuiCol_HeaderHovered]         = COLOR_GRAY;
        colors[ImGuiCol_HeaderActive]          = COLOR_TRANS_GRAY_ALT;
        colors[ImGuiCol_Separator]             = COLOR_ACCENT;
        colors[ImGuiCol_SeparatorHovered]      = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_SeparatorActive]       = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_ResizeGrip]            = COLOR_ACCENT;
        colors[ImGuiCol_ResizeGripHovered]     = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_ResizeGripActive]      = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_Tab]                   = COLOR_ACCENT_DARK;
        colors[ImGuiCol_TabHovered]            = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_TabActive]             = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_TabUnfocused]          = COLOR_ACCENT_DARK;
        colors[ImGuiCol_TabUnfocusedActive]    = COLOR_ACCENT_DARK;
        colors[ImGuiCol_DockingPreview]        = COLOR_ACCENT;
        colors[ImGuiCol_DockingEmptyBg]        = COLOR_GRAY_DARK;
        colors[ImGuiCol_TextSelectedBg]        = COLOR_ACCENT;
        colors[ImGuiCol_DragDropTarget]        = COLOR_ACCENT;
        colors[ImGuiCol_NavHighlight]          = COLOR_ACCENT;
        colors[ImGuiCol_NavWindowingHighlight] = COLOR_TRANS_WHITE;
        colors[ImGuiCol_NavWindowingDimBg]     = COLOR_TRANS_GRAY_LIGHT;
        colors[ImGuiCol_ModalWindowDimBg]      = COLOR_TRANS_GRAY_LIGHT_ALT;
    }
} //end namespace nc::ui
