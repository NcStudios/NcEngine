#pragma once

#include "ncmath/Geometry.h"

namespace nc
{
inline auto operator==(const Triangle& lhs, const Triangle& rhs) -> bool
{
    return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c;
}
} // namespace nc
