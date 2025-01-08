/**
 * @file SpringSettings.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

namespace nc
{
/** @brief Settings for softening constraints with a spring-damper. */
struct SpringSettings
{
    /** @brief Spring frequency value hints */
    struct Frequency
    {
        static constexpr auto Disabled = 0.0f; ///< disable the spring
        static constexpr auto Soft = 2.0f;     ///< typical soft spring
        static constexpr auto Stiff = 20.0f;   ///< typical stiff spring
        static constexpr auto Max = 30.0f;     ///< frequency should not exceed half simulation frequency (assumes 60 fps here)
    };

    /** @brief Spring damping value hints */
    struct Damping
    {
        static constexpr auto Undamped = 0.0f;    ///< minimal energy loss, maximum oscillation (not completely lossless for stability)
        static constexpr auto Underdamped = 0.5f; ///< some energy loss, moderate oscillation (underdamping applies when 0 < d < 1)
        static constexpr auto Critical = 1.0f;    ///< full energy loss, no oscillation
        static constexpr auto Overdamped = 2.0f;  ///< energy loss occurs even outside of oscillation (overdamping applies when d > 1)
    };

    float frequency = Frequency::Disabled; ///< oscillation rate in hertz [0, targetFPS / 2]
    float damping = Damping::Undamped;     ///< oscillation decay [0, 1] (typical max is 1, but larger values are allowed)
};
} // namespace nc
