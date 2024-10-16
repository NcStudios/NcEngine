/**
 * @file ComponentId.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <cstddef>

namespace nc
{
/** @brief Marks the beginning of the component, module, and task id range reserved for engine use. */
constexpr size_t EngineIdRangeBegin = 1ull;

/** @brief Marks the end of the component, module, and task id range reserved for engine use. */
constexpr size_t EngineIdRangeEnd = 100ull;

/** @{ */
/** @brief Unique engine component id. */
constexpr size_t FreeComponentGroupId = 1ull;
constexpr size_t TagId = 2ull;
constexpr size_t TransformId = 3ull;
constexpr size_t MeshRendererId = 4ull;
constexpr size_t ToonRendererId = 5ull;
constexpr size_t PointLightId = 6ull;
constexpr size_t SpotLightId = 7ull;
constexpr size_t SkeletalAnimatorId = 8ull;
constexpr size_t ParticleEmitterId = 9ull;
constexpr size_t RigidBodyId = 10ull;
constexpr size_t AudioSourceId = 11ull;
constexpr size_t FrameLogicId = 12ull;
constexpr size_t CollisionListenerId = 13ull;
constexpr size_t NetworkDispatcherId = 14ull;
constexpr size_t WireframeRendererId = 15ull;
constexpr size_t HierarchyId = 16ull;
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
constexpr size_t NcWindowId = 9ull;
/** @} */

namespace update_task_id
{
/** @brief Unique engine task id for update phase. */
constexpr size_t DebugRendererNewFrame = 1ull;
constexpr size_t ParticleEmitterUpdate = 2ull;
constexpr size_t AudioSourceUpdate = 3ull;
constexpr size_t FrameLogicUpdate = 4ull; // Depends on DebugRendererNewFrame
constexpr size_t PhysicsPipeline = 5ull; // Depends on FrameLogicUpdate
constexpr size_t CommitStagedChanges = 6ull; // Depends on all other update tasks
constexpr size_t ParticleEmitterSync = 7ull; // Depends on CommitStagedChanges
/** @} */
} // namespace update_task_id

namespace render_task_id
{
/** @brief Unique engine task id for render phase. */
constexpr size_t Render = 50ull;
/** @} */
} // namespace render_task_id
} // namespace nc
