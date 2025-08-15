/**
 * @file Curve.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include <cstdint>

namespace nc
{
/** @brief Common types of curves. */
enum class CurveType : uint8_t
{
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    Spike,
    Constant
};
} // namespace nc
