#pragma once

#include <cstdint>
#include <vector>

namespace nc::graphics
{
    // Note: If we want to extend this to blend between states, we need two indices and a lerp factor here.
    struct ShapeKeyAnimationRenderState
    {
        std::vector<uint32_t> staticShapeKeyMetadataIndex;
        std::vector<uint32_t> staticShapeKeyMetadataIndex;
    };
}