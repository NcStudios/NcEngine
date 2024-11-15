#pragma once

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
class InstanceCache
{
    public:
        explicit InstanceCache(uint32_t maxMeshRenderers)
            : m_maxIndex{maxMeshRenderers}
        {
        }

        auto AddInstance(Entity entity, MaterialInstanceHandle materialId) -> uint32_t
        {
            if (m_freeList.empty())
            {
                NC_ASSERT(m_nextIndex < m_maxIndex, "Max MeshRenderer instances exceeded");
                m_data.emplace_back(DirectX::XMMATRIX{}, materialId);
                m_entities.push_back(entity);
                m_dirty.push_back(m_nextIndex);
                return m_nextIndex++;
            }

            const auto index = m_freeList.back();
            m_freeList.pop_back();
            m_data[index].materialIndex = materialId;
            m_entities[index] = entity;
            m_dirty.push_back(index);
            return index;
        }

        // todo: returned index is stable - give to mesh renderer and use here instead of search...

        void RemoveInstance(Entity entity, uint32_t instance)
        {
            (void)entity;
            (void)instance;
            const auto pos = std::ranges::find(m_entities, entity);
            NC_ASSERT(pos != m_entities.end(), "Instance not found");
            const auto index = static_cast<uint32_t>(std::distance(m_entities.begin(), pos));
            m_entities[index] = Entity::Null();
            m_freeList.push_back(index);

            // NC_ASSERT(instance < m_entities.size(), "Instance out of bounds");
            // m_entities[instance] = Entity::Null();
            // m_freeList.push_back(instance);
        }

        void Clear() noexcept
        {
            m_data.clear();
            m_data.shrink_to_fit();
            m_entities.clear();
            m_entities.shrink_to_fit();
            m_dirty.clear();
            m_dirty.shrink_to_fit();
            m_freeList.clear();
            m_freeList.shrink_to_fit();
            m_nextIndex = 0;
        }

        auto HasPendingChanges() const -> bool
        {
            return !m_dirty.empty();
        }

        void UpdateMatrices(ecs::ExplicitEcs<Transform> ecs)
        {
            NC_PROFILE_SCOPE("InstanceCache::PopulateMatrices()", ProfileCategory::Rendering);
            // For all new static instances, grab matrix once
            for (const auto index : m_dirty)
            {
                const auto entity = m_entities[index];
                if (entity.IsStatic())
                {
                    const auto& transform = ecs.Get<Transform>(entity);
                    m_data[index].modelMatrix = transform.TransformationMatrix();
                }
            }

            // For each dynamic instance, update matrix and mark dirty
            for (auto i = 0u; i < m_nextIndex; ++i)
            {
                const auto entity = m_entities[i];
                if (entity.IsStatic() || !entity.Valid())
                {
                    continue;
                }

                const auto& transform = ecs.Get<Transform>(entity);
                m_data[i].modelMatrix = transform.TransformationMatrix();
                m_dirty.push_back(i);
            }
        }

        auto BuildState() -> BufferUpdateInfo<MeshRendererData>
        {
            NC_PROFILE_SCOPE("InstanceCache::BuildState()", ProfileCategory::Rendering);
            return HasPendingChanges()
                ? BufferUpdateInfo<MeshRendererData>{m_data, CollectDirtyRanges()}
                : BufferUpdateInfo<MeshRendererData>{};
        }

        // todo: RebuildStatics

    private:
        std::vector<MeshRendererData> m_data;
        std::vector<Entity> m_entities;
        std::vector<uint32_t> m_dirty;
        std::vector<uint32_t> m_freeList;
        uint32_t m_nextIndex = 0;
        uint32_t m_maxIndex;

        auto CollectDirtyRanges() -> std::vector<BufferSlice>
        {
            auto out = std::vector<BufferSlice>{};
            if (m_dirty.empty())
            {
                return out;
            }

            std::ranges::sort(m_dirty);
            auto removed = std::ranges::unique(m_dirty);
            m_dirty.erase(removed.begin(), removed.end());
            auto start = m_dirty[0];
            auto end = start + 1;
            for (const auto nextEnd : std::views::drop(m_dirty, 1))
            {
                if (nextEnd == end)
                {
                    end += 1;
                }
                else
                {
                    out.emplace_back(start, end - start);
                    start = nextEnd;
                    end = start + 1;
                }
            }

            out.emplace_back(start, end - start);
            m_dirty.clear();
            return out;
        }
};
} // namespace nc::graphics
