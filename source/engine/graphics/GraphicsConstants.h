#pragma once

#include <cstdint>

namespace nc::graphics
{
    /** How many frames can be rendered concurrently. */
    constexpr uint32_t MaxFramesInFlight = 2u;
}