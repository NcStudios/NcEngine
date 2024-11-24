#pragma once

#include "HostBuffer.h"
#include "graphics2/ShaderTypes.h"

#include <vector>


#include <ranges>
#include <algorithm>
#include "ncengine/ecs/Entity.h"
#include "ncutility/NcError.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/debug/Profile.h"


namespace nc::graphics
{
class TransformCache
{
    public:
        explicit TransformCache(uint32_t maxMeshRenderers)
            : m_buffer{maxMeshRenderers}
        {
        }

        // 
        auto AddInstance(Entity entity) -> uint32_t
        {
            const auto index = m_buffer.GetStagingArea().Emplace(DirectX::XMMATRIX{});
            if (index >= m_entities.size())
            {
                m_entities.push_back(entity);
            }
            else
            {
                m_entities[index] = entity;
            }

            return index;
        }

        void RemoveInstance(uint32_t instance)
        {
            NC_ASSERT(instance < m_entities.size(), "Instance out of bounds");
            m_buffer.GetStagingArea().Erase(instance);
            m_entities[instance] = Entity::Null();
        }

        void Clear() noexcept
        {
            m_buffer.Clear();
            m_entities.clear();
            m_entities.shrink_to_fit();
        }

        void CommitPendingChanges()
        {
            m_buffer.CommitPendingChanges();
        }

        // todo? this could be parallel:
        //  - update statics on this thread
        //  - fire async task tot update range of like 32k at a time
        void UpdateMatrices(ecs::ExplicitEcs<Transform> ecs)
        {
            NC_PROFILE_SCOPE("TransformCache::UpdateMatrices()", ProfileCategory::Rendering);

            // For all new static instances, grab matrix once
            for (const auto index : m_buffer.GetDirtyIndices())
            {
                const auto entity = m_entities[index];
                if (entity.IsStatic())
                {
                    const auto& transform = ecs.Get<Transform>(entity);
                    // ahh! this is already a dirty index...
                    m_buffer.AccessForWrite(index).modelMatrix = transform.TransformationMatrix();
                }
            }

            // things to try for performance:
            // - what is cost of just updating all vs. updating dirty?
            // - what happens if we store FLOAT4X4 instead?
            // - I assume its ecs.Get() taking all of the time. Is there a way to look closer at to confirm?


            auto& pool = ecs.GetPool<Transform>();

            // For each dynamic instance, update matrix and mark dirty
            for (auto [i, entity] : std::views::enumerate(m_entities))
            {
                if (entity.IsStatic() || !entity.Valid())
                {
                    continue;
                }

                const auto handle = static_cast<HostBufferHandle>(i);
                const auto& transform = pool.Get(entity);
                m_buffer.AccessForWrite(handle).modelMatrix = transform.TransformationMatrix();
            }
        }

        auto BuildState() -> BufferUpdateInfo<TransformData>
        {
            NC_PROFILE_SCOPE("TransformCache::BuildState()", ProfileCategory::Rendering);
            return m_buffer.BuildUpdateInfo();
        }

        void MarkStaticsDirty()
        {
            for (auto [i, entity] : std::views::enumerate(m_entities))
            {
                if (entity.IsStatic() || !entity.Valid())
                {
                    const auto handle = static_cast<HostBufferHandle>(i);
                    m_buffer.MarkDirty(handle);
                }
            }
        }

    private:
        HostBuffer<TransformData> m_buffer;
        std::vector<Entity> m_entities;
};
} // namespace nc::graphics
