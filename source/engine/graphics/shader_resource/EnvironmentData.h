#pragma once

#include "ncmath/Vector.h"

#include <string>

namespace nc::graphics
{
struct EnvironmentData
{
    Vector3 cameraWorldPosition = Vector3(0.0f, 0.0f, 0.0f);
    uint32_t skyboxTextureIndex = 0u;
};
} // namespace nc::graphics
