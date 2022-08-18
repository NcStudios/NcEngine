#pragma once

namespace nc::time
{
/** @brief The number of seconds between the previous frame and the current one. */
auto DeltaTime() noexcept -> float;
}