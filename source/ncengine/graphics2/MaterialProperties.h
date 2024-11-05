#pragma once

#include "ncmath/Vector.h"

#include <limits>
#include <span>
#include <vector>

namespace nc::graphics
{
// MaterialInstance representation for use in a SturcturedBuffer.
struct MaterialProperties
{
    uint32_t diffuseTexIndex = std::numeric_limits<uint32_t>::max();
    uint32_t normalTexIndex = std::numeric_limits<uint32_t>::max();
    Vector3 gradientStart = Vector3::One();
    Vector3 gradientEnd = Vector3::One();
    Vector3 outlineColor = Vector3::One();
    float outlineWidth = 1.0f;
};

// Indicates a dirty subrange within the MaterialProperties buffer.
struct UpdateRange
{
    size_t begin = 0;
    size_t end = 0;
};

// Event data for notifying changes to MaterialInstances.
struct MaterialPropertyUpdateInfo
{
    std::span<const MaterialProperties> instances; // view over all instances
    std::vector<UpdateRange> dirtyRanges;          // subranges that have been modified
};
} // namespace nc::graphics
