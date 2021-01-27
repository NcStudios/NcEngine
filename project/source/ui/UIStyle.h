#pragma once

#include "imgui/imgui.h"

namespace project::ui
{
    const ImVec4 COLOR_WHITE                (1.000f, 1.000f, 1.000f, 1.000f);
    const ImVec4 COLOR_TRANS_WHITE          (1.000f, 1.000f, 1.000f, 0.700f);
    const ImVec4 COLOR_TRANS                (0.000f, 0.000f, 0.000f, 0.000f);
    const ImVec4 COLOR_TRANS_GRAY_DARK      (0.020f, 0.020f, 0.020f, 0.530f);
    const ImVec4 COLOR_TRANS_GRAY           (0.430f, 0.430f, 0.500f, 0.500f);
    const ImVec4 COLOR_TRANS_GRAY_ALT       (0.670f, 0.670f, 0.670f, 0.390f);
    const ImVec4 COLOR_TRANS_GRAY_LIGHT     (0.800f, 0.800f, 0.800f, 0.200f);
    const ImVec4 COLOR_TRANS_GRAY_LIGHT_ALT (0.800f, 0.800f, 0.800f, 0.350f);
    const ImVec4 COLOR_GRAY_DARK            (0.114f, 0.125f, 0.133f, 1.000f);
    const ImVec4 COLOR_GRAY                 (0.200f, 0.200f, 0.200f, 1.000f);
    const ImVec4 COLOR_GRAY_DARKEST         (0.067f, 0.067f, 0.067f, 1.000f);
    const ImVec4 COLOR_GRAY_LIGHT           (0.380f, 0.380f, 0.380f, 1.000f);
    const ImVec4 COLOR_GRAY_LIGHTEST        (0.500f, 0.500f, 0.500f, 1.000f);
    const ImVec4 COLOR_ACCENT_DARK          (0.000f, 0.447f, 0.000f, 1.000f);
    const ImVec4 COLOR_ACCENT               (0.000f, 0.490f, 0.000f, 1.000f);
    const ImVec4 COLOR_ACCENT_LIGHT         (0.000f, 0.729f, 0.000f, 1.000f);


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
        colors[ImGuiCol_WindowBg]              = COLOR_GRAY_DARK;
        colors[ImGuiCol_ChildBg]               = COLOR_GRAY_DARK;
        colors[ImGuiCol_PopupBg]               = COLOR_GRAY_DARK;
        colors[ImGuiCol_Border]                = COLOR_ACCENT;
        colors[ImGuiCol_BorderShadow]          = COLOR_TRANS;
        colors[ImGuiCol_FrameBg]               = COLOR_GRAY_DARKEST;
        colors[ImGuiCol_FrameBgHovered]        = COLOR_GRAY_LIGHT;
        colors[ImGuiCol_FrameBgActive]         = COLOR_TRANS_GRAY_ALT;
        colors[ImGuiCol_TitleBg]               = COLOR_ACCENT_DARK;
        colors[ImGuiCol_TitleBgActive]         = COLOR_ACCENT_DARK;
        colors[ImGuiCol_TitleBgCollapsed]      = COLOR_GRAY_DARK;
        colors[ImGuiCol_MenuBarBg]             = COLOR_GRAY_DARK;
        colors[ImGuiCol_ScrollbarBg]           = COLOR_TRANS_GRAY_DARK;
        colors[ImGuiCol_ScrollbarGrab]         = COLOR_ACCENT;
        colors[ImGuiCol_ScrollbarGrabHovered]  = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_ScrollbarGrabActive]   = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_CheckMark]             = COLOR_ACCENT;
        colors[ImGuiCol_SliderGrab]            = COLOR_ACCENT;
        colors[ImGuiCol_SliderGrabActive]      = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_Button]                = COLOR_GRAY_DARKEST;
        colors[ImGuiCol_ButtonHovered]         = COLOR_GRAY_LIGHT;
        colors[ImGuiCol_ButtonActive]          = COLOR_TRANS_GRAY_ALT;
        colors[ImGuiCol_Header]                = COLOR_GRAY_DARK;
        colors[ImGuiCol_HeaderHovered]         = COLOR_GRAY;
        colors[ImGuiCol_HeaderActive]          = COLOR_GRAY_LIGHT;
        colors[ImGuiCol_Separator]             = COLOR_TRANS_GRAY;
        colors[ImGuiCol_SeparatorHovered]      = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_SeparatorActive]       = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_ResizeGrip]            = COLOR_ACCENT;
        colors[ImGuiCol_ResizeGripHovered]     = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_ResizeGripActive]      = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_Tab]                   = COLOR_GRAY_DARKEST;
        colors[ImGuiCol_TabHovered]            = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_TabActive]             = COLOR_ACCENT_LIGHT;
        colors[ImGuiCol_TabUnfocused]          = COLOR_ACCENT_DARK;
        colors[ImGuiCol_TabUnfocusedActive]    = COLOR_ACCENT_DARK;
        colors[ImGuiCol_DockingPreview]        = COLOR_ACCENT;
        colors[ImGuiCol_DockingEmptyBg]        = COLOR_GRAY;
        colors[ImGuiCol_TextSelectedBg]        = COLOR_ACCENT;
        colors[ImGuiCol_DragDropTarget]        = COLOR_ACCENT;
        colors[ImGuiCol_NavHighlight]          = COLOR_ACCENT;
        colors[ImGuiCol_NavWindowingHighlight] = COLOR_TRANS_WHITE;
        colors[ImGuiCol_NavWindowingDimBg]     = COLOR_TRANS_GRAY_LIGHT;
        colors[ImGuiCol_ModalWindowDimBg]      = COLOR_TRANS_GRAY_LIGHT_ALT;
        colors[ImGuiCol_PlotHistogram]         = COLOR_ACCENT_DARK;
        colors[ImGuiCol_PlotHistogramHovered]  = COLOR_ACCENT;
    }
} //end namespace nc::ui
