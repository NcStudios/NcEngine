#pragma once

#include "HostStructuredBuffer.h"

#include "ncengine/ecs/EcsFwd.h"

#include <vector>

namespace nc
{
class Entity;
class Transform;

namespace graphics
{
// Unique identifier for a TransformData instance.
using TransformDataHandle = HostStructuredBufferHandle;

// CPU-side buffer for Transform matrices. Effectively has the same behavior as a StructuredHostBuffer, but needs to be
// synced with the registry each frame via UpdateMatrices(), which may NOT run concurrently with game logic.
class TransformCache
{
    public:
        explicit TransformCache(uint32_t maxMeshRenderers)
            : m_buffer{maxMeshRenderers}
        {
        }

        // API-Facing Functions
        auto AddInstance(Entity entity) -> TransformDataHandle;
        void RemoveInstance(TransformDataHandle instance);

        // Graphics Frontend Functions
        void CommitPendingChanges();
        void UpdateMatrices(ecs::ExplicitEcs<Transform> ecs);
        auto BuildState() -> BufferUpdateInfo<TransformData>;
        void MarkStaticsDirty();
        auto GetEntity(TransformDataHandle handle) const -> Entity;
        auto GetInstance(TransformDataHandle handle) const -> const TransformData&;

    private:
        HostStructuredBuffer<TransformData> m_buffer;
        std::vector<Entity> m_entities;
};
} // namespace graphics
} // namespace nc
