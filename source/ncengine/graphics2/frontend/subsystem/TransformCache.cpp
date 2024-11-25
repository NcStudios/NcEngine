#include "TransformCache.h"

#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/Ecs.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace nc::graphics
{
auto TransformCache::AddInstance(Entity entity) -> TransformDataHandle
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

void TransformCache::RemoveInstance(TransformDataHandle instance)
{
    NC_ASSERT(instance < m_entities.size(), "Instance out of bounds");
    m_buffer.GetStagingArea().Erase(instance);
    m_entities[instance] = Entity::Null();
}

void TransformCache::CommitPendingChanges()
{
    m_buffer.CommitPendingChanges();
}

void TransformCache::UpdateMatrices(ecs::ExplicitEcs<Transform> ecs)
{
    NC_PROFILE_SCOPE("TransformCache::UpdateMatrices()", ProfileCategory::Rendering);

    auto data = m_buffer.data();
    auto& pool = ecs.GetPool<Transform>();

    // For all new static instances, grab matrix first time.
    for (const auto index : m_buffer.GetDirtyIndices())
    {
        const auto entity = m_entities[index];
        if (entity.IsStatic())
        {
            const auto& transform = pool.Get(entity);
            data[index].matrix = transform.TransformationMatrix();
        }
    }

    // For each dynamic instance, update matrix and mark dirty
    for (auto [i, entity] : std::views::enumerate(m_entities))
    {
        if (entity.IsStatic() || !entity.Valid())
        {
            continue;
        }

        const auto handle = static_cast<TransformDataHandle>(i);
        data[handle].matrix = pool.Get(entity).TransformationMatrix();
        m_buffer.MarkDirty(handle);
    }
}

auto TransformCache::BuildState() -> BufferUpdateInfo<TransformData>
{
    NC_PROFILE_SCOPE("TransformCache::BuildState()", ProfileCategory::Rendering);
    return m_buffer.BuildUpdateInfo();
}

void TransformCache::MarkStaticsDirty()
{
    for (auto [i, entity] : std::views::enumerate(m_entities))
    {
        if (entity.IsStatic())
        {
            m_buffer.MarkDirty(static_cast<TransformDataHandle>(i));
        }
    }
}

auto TransformCache::GetEntity(TransformDataHandle handle) const -> Entity
{
    return m_entities.at(handle);
}

auto TransformCache::GetInstance(TransformDataHandle handle) const -> const TransformData&
{
    return m_buffer.AccessForRead(handle);
}
} // namespace nc::graphics
