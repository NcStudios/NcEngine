#include "UIPosition.h"
#include "imgui/imgui.h"

namespace nc::ui::utils
{
    ImVec2 GetTopLeftCoords(UIPosition position, const ImVec2& screenDimensions, const ImVec2& elementDimensions)
    {
        switch(position)
        {
            case UIPosition::Center:
            {
                return { (screenDimensions.x - elementDimensions.x) / 2.0f,
                         (screenDimensions.y - elementDimensions.y) / 2.0f };
            }
            case UIPosition::TopLeft:
            {
                return { 0.0f, 0.0f };
            }
            case UIPosition::TopCenter:
            {
                return { (screenDimensions.x - elementDimensions.x) / 2.0f, 0.0f };
            }
            case UIPosition::TopRight:
            {
                return { screenDimensions.x - elementDimensions.x, 0.0f };
            }
            case UIPosition::BottomLeft:
            {
                return { 0.0f, screenDimensions.y - elementDimensions.y };
            }
            case UIPosition::BottomCenter:
            {
                return { (screenDimensions.x - elementDimensions.x) / 2.0f,
                          screenDimensions.y - elementDimensions.y };
            }
            case UIPosition::BottomRight:
            {
                return { screenDimensions.x - elementDimensions.x,
                         screenDimensions.y - elementDimensions.y };
            }
            case UIPosition::LeftCenter:
            {
                return { 0.0f, (screenDimensions.y - elementDimensions.y) / 2.0f };
            }
            case UIPosition::RightCenter:
            {
                return {  screenDimensions.x - elementDimensions.x,
                         (screenDimensions.y - elementDimensions.y) / 2.0f };
            }
            default:
                throw std::runtime_error("Invalid UIPosition");
        }
    }
} // end namespace nc::ui::utils