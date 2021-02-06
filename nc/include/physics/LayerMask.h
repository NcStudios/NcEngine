#pragma once

#include <stdint.h>

namespace nc::physics
{
    using LayerMask = uint64_t;
    constexpr LayerMask LAYER_MASK_NONE = 0u;
    constexpr LayerMask LAYER_MASK_ALL = ~LAYER_MASK_NONE;

    namespace Layer
    {
        const auto Ground = 1u;
        const auto GamePiece = 2u;
    }
}