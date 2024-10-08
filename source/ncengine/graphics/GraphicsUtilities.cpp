#include "GraphicsUtilities.h"

namespace nc::graphics
{
auto AdjustDimensionsToAspectRatio(const nc::Vector2& dimensions) -> nc::Vector2
{
    auto width = dimensions.x;
    auto height = dimensions.y;

    if (FloatEqual(height, 0.0f))
    {
        height = 0.00000001f;
    }

    auto currentAspectRatio = width / height;
    if (currentAspectRatio > AspectRatio)
    {
        width = AspectRatio * height;
    }
    else
    {
        height = width / AspectRatio;
    }
    return nc::Vector2{width, height};
}
} // namespace nc::graphics
