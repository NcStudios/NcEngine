/**
 * @file Color.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncmath/Vector.h"

namespace nc
{
/** @brief Four component color. */
struct Color : public Vector4
{
    using Vector4::Vector4;

    constexpr explicit Color(const Vector4& in)
        : Vector4{in}
    {
    }

    constexpr explicit Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : Vector4{(float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f}
    {
    }

    static constexpr auto Red()         -> Color { return Color{1.0f, 0.0, 0.0f, 1.0f}; }
    static constexpr auto Green()       -> Color { return Color{0.0f, 1.0, 0.0f, 1.0f}; }
    static constexpr auto Blue()        -> Color { return Color{0.0f, 0.0, 1.0f, 1.0f}; }
    static constexpr auto White()       -> Color { return Color{1.0f, 1.0, 1.0f, 1.0f}; }
    static constexpr auto Black()       -> Color { return Color{0.0f, 0.0, 0.0f, 1.0f}; }
    static constexpr auto Transparent() -> Color { return Color{0.0f, 0.0, 0.0f, 0.0f}; }

    constexpr auto R()       ->       float& { return x; }
    constexpr auto G()       ->       float& { return y; }
    constexpr auto B()       ->       float& { return z; }
    constexpr auto A()       ->       float& { return w; }
    constexpr auto R() const -> const float& { return x; }
    constexpr auto G() const -> const float& { return y; }
    constexpr auto B() const -> const float& { return z; }
    constexpr auto A() const -> const float& { return w; }
};

/** @brief Gradient between two Colors. */
struct Gradient
{
    Color start;
    Color end;

    constexpr Gradient() noexcept
        : Gradient{Color::White()}
    {
    }

    constexpr Gradient(const Color& solidColor) noexcept
        : start{solidColor}, end{solidColor}
    {
    }

    constexpr Gradient(const Color& from, const Color& to) noexcept
        : start{from}, end{to}
    {
    }

    /** @brief Select a Color between start and end. */
    constexpr auto Lerp(float t) const noexcept -> Color
    {
        return Color{
            nc::Lerp(start.R(), end.R(), t),
            nc::Lerp(start.G(), end.G(), t),
            nc::Lerp(start.B(), end.B(), t),
            nc::Lerp(start.A(), end.A(), t)
        };
    }

    /** @brief Select a Color between start and end with distinct color and alpha factors. */
    constexpr auto Lerp(float tColor, float tAlpha) const noexcept -> Color
    {
        return Color{
            nc::Lerp(start.R(), end.R(), tColor),
            nc::Lerp(start.G(), end.G(), tColor),
            nc::Lerp(start.B(), end.B(), tColor),
            nc::Lerp(start.A(), end.A(), tAlpha)
        };
    }

    friend constexpr auto operator==(const Gradient& lhs, const Gradient& rhs) -> bool
    {
        return lhs.start == rhs.start && lhs.end == rhs.end;
    }
};
} // namespace nc
