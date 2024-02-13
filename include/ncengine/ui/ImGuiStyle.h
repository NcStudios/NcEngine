/**
 * @file ImGuiStyle.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include "ncmath/Vector.h"

namespace nc::ui
{
/** @brief Set ImGUi style using values from 'default_scheme'.*/
void SetDefaultUIStyle();

namespace color
{
constexpr auto Black = Vector4{0.009f, 0.009f, 0.009f, 1.000f};
constexpr auto White = Vector4{1.000f, 1.000f, 1.000f, 1.000f};
constexpr auto Clear = Vector4{0.000f, 0.000f, 0.000f, 0.000f};
constexpr auto Red   = Vector4{1.000f, 0.200f, 0.100f, 1.000f};
constexpr auto Green = Vector4{0.000f, 1.000f, 0.000f, 1.000f};
constexpr auto Blue  = Vector4{0.000f, 0.400f, 1.000f, 1.000f};
} // namespace color

namespace default_scheme
{
constexpr auto Title           = color::Black;
constexpr auto Border          = color::Black;
constexpr auto Text            = color::White;
constexpr auto Button          = color::Black;
constexpr auto Separator       = Vector4{0.410f, 0.410f, 0.410f, 0.500f};
constexpr auto Tabs            = Vector4{0.072f, 0.072f, 0.072f, 0.750f};
constexpr auto WindowBG        = Vector4{0.018f, 0.018f, 0.018f, 0.750f};
constexpr auto ChildBG         = Vector4{0.035f, 0.035f, 0.035f, 0.750f};
constexpr auto PopupBG         = Vector4{0.018f, 0.018f, 0.018f, 1.000f};
constexpr auto ScrollBarBG     = Vector4{0.017f, 0.017f, 0.017f, 0.530f};
constexpr auto DimBg           = Vector4{0.740f, 0.740f, 0.740f, 0.350f};
constexpr auto WindowHighlight = Vector4{1.000f, 1.000f, 1.000f, 0.700f};
constexpr auto Active          = Vector4{0.610f, 0.610f, 0.610f, 0.390f};
constexpr auto Hovered         = Vector4{0.0f,  0.0f,   0.0f,   0.590f};
constexpr auto Disabled        = Vector4{0.348f, 0.348f, 0.348f, 1.000f};
constexpr auto Accent          = Vector4{0.000f, 0.490f, 0.000f, 1.000f};
constexpr auto AccentActive    = Vector4{0.000f, 0.729f, 0.000f, 1.000f};
constexpr auto AccentTrans     = Vector4{0.000f, 0.990f, 0.000f, 0.050f};
constexpr auto AccentDim       = Vector4{0.000f, 0.447f, 0.000f, 0.750f};
} // namespace default_scheme
} // namespace nc::ui
