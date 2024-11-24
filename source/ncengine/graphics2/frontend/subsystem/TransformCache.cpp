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

void TransformCache::Clear() noexcept
{
    m_buffer.Clear();
    m_entities.clear();
    m_entities.shrink_to_fit();
}

void TransformCache::CommitPendingChanges()
{
    m_buffer.CommitPendingChanges();
}

// todo? this could be parallel:
//  - update statics on this thread
//  - fire async task tot update range of like 32k at a time
void TransformCache::UpdateMatrices(ecs::ExplicitEcs<Transform> ecs)
{
    NC_PROFILE_SCOPE("TransformCache::UpdateMatrices()", ProfileCategory::Rendering);

    auto data = m_buffer.data();
    auto& pool = ecs.GetPool<Transform>();

    // For all new static instances, grab matrix once
    for (const auto index : m_buffer.GetDirtyIndices())
    {
        const auto entity = m_entities[index];
        if (entity.IsStatic())
        {
            const auto& transform = pool.Get(entity);
            data[index].modelMatrix = transform.TransformationMatrix();
        }
    }

    // things to try for performance:
    // - what is cost of just updating all vs. updating dirty?
    // - what happens if we store FLOAT4X4 instead?
    // - I assume its ecs.Get() taking all of the time. Is there a way to look closer at to confirm?


    // For each dynamic instance, update matrix and mark dirty
    for (auto [i, entity] : std::views::enumerate(m_entities))
    {
        if (entity.IsStatic() || !entity.Valid())
        {
            continue;
        }

        const auto handle = static_cast<TransformDataHandle>(i);
        data[handle].modelMatrix = pool.Get(entity).TransformationMatrix();
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
} // namespace nc::graphics
