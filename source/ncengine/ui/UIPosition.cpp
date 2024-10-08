#include "ui/UIPosition.h"
#include "ncutility/NcError.h"

namespace nc::ui::utils
{
    Vector2 GetTopLeftCoords(UIPosition position, const Vector2& screenExtent, const Vector2& elementDimensions)
    {
        switch(position)
        {
            case UIPosition::TopLeft:
            {
                return Vector2{ 0.0f, 0.0f };
            }
            case UIPosition::TopCenter:
            {
                return Vector2{ (screenExtent.x - elementDimensions.x) / 2.0f, 0.0f };
            }
            case UIPosition::TopRight:
            {
                return Vector2{ screenExtent.x - elementDimensions.x, 0.0f };
            }
            case UIPosition::LeftCenter:
            {
                return Vector2{ 0.0f, (screenExtent.y - elementDimensions.y) / 2.0f };
            }
            case UIPosition::Center:
            {
                return Vector2{ (screenExtent.x - elementDimensions.x) / 2.0f,
                                (screenExtent.y - elementDimensions.y) / 2.0f };
            }
            case UIPosition::RightCenter:
            {
                return Vector2{ screenExtent.x - elementDimensions.x,
                                (screenExtent.y - elementDimensions.y) / 2.0f };
            }
            case UIPosition::BottomLeft:
            {
                return Vector2{ 0.0f, screenExtent.y - elementDimensions.y };
            }
            case UIPosition::BottomCenter:
            {
                return Vector2{ (screenExtent.x - elementDimensions.x) / 2.0f,
                                 screenExtent.y - elementDimensions.y };
            }
            case UIPosition::BottomRight:
            {
                return Vector2{ screenExtent.x - elementDimensions.x,
                                screenExtent.y - elementDimensions.y };
            }
            default:
                throw NcError("Invalid UIPosition");
        }
    }
} // end namespace nc::ui::utils