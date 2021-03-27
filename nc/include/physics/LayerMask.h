#pragma once

#include <stdint.h>

namespace nc::physics
{
    /** Defined layers in project code as powers of 2 however desired - enum, constants, etc. */
    using Layer = uint64_t;
    using LayerMask = uint64_t;
    using LayerMask32 = uint32_t;
    constexpr Layer DefaultLayer = 1u;
    constexpr LayerMask LayerMaskNone = 0u;
    constexpr LayerMask LayerMaskAll = ~LayerMaskNone;
    constexpr LayerMask32 LayerMask32None = 0u;
    constexpr LayerMask32 LayerMask32All = ~LayerMask32None;
}