/**
 * @file SkeletalAnimationController.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <cstdint>
#include <limits>

namespace nc
{
/** @brief Null animation asset identifier. */
constexpr auto NullAnimationId = std::numeric_limits<uint64_t>::max();

/** @todo 832 Migrate state machine code here. */
class SkeletalAnimationController
{
    public:
        explicit SkeletalAnimationController(uint64_t) {}
};
} // namespace nc
