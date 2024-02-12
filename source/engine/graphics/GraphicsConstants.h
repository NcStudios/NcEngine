#pragma once

#include <cstdint>

namespace nc::graphics
{
    /** How many frames can be rendered concurrently. Controls count of buffers, images, descriptor sets, synchronization constants, etc. */
    constexpr uint32_t MaxFramesInFlight = 2u;

    /** Starting point for how many resources can be bound per shader. Arrays, for example, count as a single slot. */
    constexpr uint32_t DefaultResourceSlotsPerShader = 10u;

    /** Max for how many resources can be bound per shader. Arrays, for example, count as a single slot. */
    constexpr uint32_t MaxResourceSlotsPerShader = 100u;
}