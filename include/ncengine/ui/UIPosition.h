#pragma once

#include "ncmath/Vector.h"

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
        Vector2 GetTopLeftCoords(UIPosition position, const Vector2& screenDimensions, const Vector2& elementDimensions);
    }
}