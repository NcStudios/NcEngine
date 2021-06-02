#pragma once

#include "Entity.h"

namespace nc::physics
{
    /** Defined layers in project code as powers of 2 however desired - enum, constants, etc. */
    using LayerMask = uint64_t;
    using LayerMask32 = uint32_t;
    constexpr EntityTraits::layer_type DefaultLayer = 1u;
    constexpr LayerMask LayerMaskNone = 0u;
    constexpr LayerMask LayerMaskAll = ~LayerMaskNone;
    constexpr LayerMask32 LayerMask32None = 0u;
    constexpr LayerMask32 LayerMask32All = ~LayerMask32None;

    template<explicit_layer_type T>
    auto ToLayerMask(T layer) -> LayerMask
    {
        //check max
        return layer == 0u ? LayerMask{0u} : LayerMask{1u} << (layer - 1u);
    }

    template<explicit_layer_type T>
    auto ToLayerMask32(T layer) -> LayerMask32
    {
        //check max
        return layer == 0u ? LayerMask32{0u} : LayerMask32{1u} << (layer - 1u);
    }
}