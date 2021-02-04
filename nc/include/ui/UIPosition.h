#pragma once

struct ImVec2;

namespace nc::ui
{
    enum class UIPosition
    {
        TopLeft,
        TopCenter,
        TopRight,
        LeftCenter,
        Center,
        RightCenter,
        BottomLeft,
        BottomCenter,
        BottomRight
    };

    namespace utils
    {
        ImVec2 GetTopLeftCoords(UIPosition position, const ImVec2& screenDimensions, const ImVec2& elementDimensions);
    }
}