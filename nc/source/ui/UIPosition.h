#pragma once

#include <stdexcept>

struct ImVec2;

namespace nc::ui
{
    enum class UIPosition
    {
        Center,
        TopLeft,
        TopCenter,
        TopRight,
        BottomLeft,
        BottomCenter,
        BottomRight,
        LeftCenter,
        RightCenter
    };

    namespace utils
    {
        ImVec2 GetTopLeftCoords(UIPosition position, const ImVec2& screenDimensions, const ImVec2& elementDimensions);
    }
}