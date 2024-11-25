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
// 
using TransformDataHandle = HostStructuredBufferHandle;

//
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

        // Frontend Functions
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
