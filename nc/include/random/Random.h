#pragma once

#include "math/Vector.h"

namespace nc::random
{
    float Float();
    float Float(float offset, float range);
    Vector3 Vec3();
    Vector3 Vec3(Vector3 offset, Vector3 range);
}