#pragma once

#include "math/Vector.h"

/** Conversions between nc and imgui vector types. IMGUI_USER_CONFIG
 *  points to this file and must be defined everywhere. */

#define IM_VEC2_CLASS_EXTRA                             \
        constexpr ImVec2(const nc::Vector2& v) noexcept \
            : x{v.x}, y{v.y}                            \
        {                                               \
        }                                               \
                                                        \
        constexpr operator nc::Vector2() const noexcept \
        {                                               \
            return nc::Vector2{x, y};                   \
        }                                               \

#define IM_VEC3_CLASS_EXTRA                             \
        constexpr ImVec3(const nc::Vector3& v) noexcept \
            : x{v.x}, y{v.y}, z{v.z}                    \
        {                                               \
        }                                               \
                                                        \
        constexpr operator nc::Vector3() const noexcept \
        {                                               \
            return nc::Vector3{x, y, z};                \
        }

#define IM_VEC4_CLASS_EXTRA                             \
        constexpr ImVec4(const nc::Vector4& v) noexcept \
            : x{v.x}, y{v.y}, z{v.z}, w{v.w}            \
        {                                               \
        }                                               \
                                                        \
        constexpr operator nc::Vector4() const noexcept \
        {                                               \
            return nc::Vector4{x, y, z, w};             \
        }
