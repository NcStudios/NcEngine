#pragma once

#include "graphics2/ShaderTypes.h"

#include <vector>


#include <ranges>
#include <algorithm>
#include "ncengine/ecs/Entity.h"
#include "ncutility/NcError.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/debug/Profile.h"

#include "HostBufferCache.h"

namespace nc::graphics
{
class TransformCache
{
    public:
        explicit TransformCache(uint32_t maxMeshRenderers)
            : m_buffer{maxMeshRenderers}
        {
        }

        auto AddInstance(Entity entity) -> uint32_t
        {
            const auto index = m_buffer.emplace(DirectX::XMMATRIX{});
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
            m_buffer.erase(instance);
            m_entities[instance] = Entity::Null();
        }

        void Clear() noexcept
        {
            m_buffer.clear();
            m_buffer.shrink_to_fit();
            m_entities.clear();
            m_entities.shrink_to_fit();
        }

        auto HasPendingChanges() const -> bool
        {
            return !m_buffer.has_dirty_indices();
        }

        // todo? this could be parallel:
        //  - update statics on this thread
        //  - fire async task tot update range of like 32k at a time
        void UpdateMatrices(ecs::ExplicitEcs<Transform> ecs)
        {
            NC_PROFILE_SCOPE("TransformCache::PopulateMatrices()", ProfileCategory::Rendering);
            // For all new static instances, grab matrix once
            for (const auto index : m_buffer.get_dirty_indices())
            {
                const auto entity = m_entities[index];
                if (entity.IsStatic())
                {
                    const auto& transform = ecs.Get<Transform>(entity);
                    m_buffer.access_for_write(index).modelMatrix = transform.TransformationMatrix();
                }
            }

            // For each dynamic instance, update matrix and mark dirty
            for (auto [i, entity] : std::views::enumerate(m_entities))
            {
                if (entity.IsStatic() || !entity.Valid())
                {
                    continue;
                }

                const auto& transform = ecs.Get<Transform>(entity);
                const auto handle = static_cast<HostBufferHandle>(i);
                m_buffer.access_for_write(handle).modelMatrix = transform.TransformationMatrix();
            }
        }

        auto BuildState() -> BufferUpdateInfo<TransformData>
        {
            NC_PROFILE_SCOPE("TransformCache::BuildState()", ProfileCategory::Rendering);
            return m_buffer.build_update_info();
        }

        void MarkStaticsDirty()
        {
            for (auto [i, entity] : std::views::enumerate(m_entities))
            {
                if (entity.IsStatic() || !entity.Valid())
                {
                    const auto handle = static_cast<HostBufferHandle>(i);
                    m_buffer.mark_dirty(handle);
                }
            }
        }

    private:
        HostBuffer<TransformData> m_buffer;
        std::vector<Entity> m_entities;
};
} // namespace nc::graphics
