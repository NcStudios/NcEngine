/**
 * @file ComponentId.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <cstddef>

namespace nc
{
/** @brief Marks the beginning of the component and module id range reserved for engine use. */
constexpr size_t EngineIdRangeBegin = 1ull;

/** @brief Marks the end of the component and module id range reserved for engine use. */
constexpr size_t EngineIdRangeEnd = 100ull;

/** @{ */
/** @brief Unique engine component id. */
constexpr size_t FreeComponentGroupId = 1ull;
constexpr size_t TagId = 2ull;
constexpr size_t TransformId = 3ull;
constexpr size_t MeshRendererId = 4ull;
constexpr size_t ToonRendererId = 5ull;
constexpr size_t PointLightId = 6ull;
constexpr size_t SkeletalAnimatorId = 7ull;
constexpr size_t ParticleEmitterId = 8ull;
constexpr size_t ColliderId = 9ull;
constexpr size_t ConcaveColliderId = 10ull;
constexpr size_t PhysicsBodyId = 11ull;
constexpr size_t FrameLogicId = 12ull;
constexpr size_t FixedLogicId = 13ull;
constexpr size_t CollisionLogicId = 14ull;
constexpr size_t AudioSourceId = 15ull;
constexpr size_t NetworkDispatcherId = 16ull;
/** @} */

/** @{ */
/** @brief Unique engine module id. */
constexpr size_t NcAssetId = 1ull;
constexpr size_t NcAudioId = 2ull;
constexpr size_t NcEcsId = 3ull;
constexpr size_t NcGraphicsId = 4ull;
constexpr size_t NcPhysicsId = 5ull;
constexpr size_t NcTimeId = 6ull;
constexpr size_t NcRandomId = 7ull;
constexpr size_t NcSceneId = 8ull;
/** @} */
} // namespace nc
