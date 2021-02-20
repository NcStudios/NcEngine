#include "ColliderSystem.h"

namespace nc::physics
{
    ColliderSystem::ColliderSystem(uint32_t maxColliders)
        : m_static{maxColliders},
          m_dynamic{maxColliders},
          m_maxColliders{maxColliders},
          m_locations{}
    {
    }

    ColliderSystem::~ColliderSystem() = default;

    ColliderSoA* ColliderSystem::GetStaticSOA()
    {
        return &m_static;
    }

    ColliderSoA* ColliderSystem::GetDynamicSOA()
    {
        return &m_dynamic;
    }

    Collider* ColliderSystem::Add(EntityHandle handle, ColliderInfo info)
    {
        auto& target = GetEntity(handle)->IsStatic ? m_static : m_dynamic;

        if(target.nextFree >= m_maxColliders) // throw?
            return nullptr;

        uint32_t targetPos = [&target]()
        {
            if(target.gaps.empty())
                return target.nextFree++;
            
            auto out = target.gaps.back();
            target.gaps.pop_back();
            return out;
        }();

        target.handles[targetPos] = static_cast<EntityHandle::Handle_t>(handle);
        target.transforms[targetPos] = &GetComponent<Transform>(handle)->GetTransformationMatrix();
        target.volumeData[targetPos] = ColliderSoA::CenterExtentPair{{info.offset.x, info.offset.y, info.offset.z}, {info.scale.x / 2.0f, info.scale.y / 2.0f, info.scale.z / 2.0f}};
        target.types[targetPos] = info.type;
        m_locations.emplace_back(handle, targetPos, &target);
        return ComponentSystem<Collider>::Add(handle, info);
    }

    bool ColliderSystem::Remove(EntityHandle handle)
    {
        if(!ComponentSystem<Collider>::Remove(handle))
            return false;

        /** @todo range is sorted - can do better than linear time? */
        auto pos = std::find_if(m_locations.cbegin(), m_locations.cend(), [handle](auto& data)
        {
            return data.handle == handle;
        });

        if(pos == m_locations.cend())
            throw std::runtime_error("ColliderSystem::Remove - ComponentSystem and SOA not synced");

        auto index = pos->index;
        auto& target = GetEntity(handle)->IsStatic ? m_static : m_dynamic;
        target.gaps.push_back(index);
        std::sort(target.gaps.begin(), target.gaps.end());
        return true;
    }

    void ColliderSystem::Clear()
    {
        ComponentSystem::Clear();
        m_dynamic.gaps.resize(0u);
        m_dynamic.nextFree = 0u;
        m_static.gaps.resize(0u);
        m_static.nextFree = 0u;
        m_locations.resize(0u);
    }
} // namespace nc::physics