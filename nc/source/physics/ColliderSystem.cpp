#include "ColliderSystem.h"
#include "config/Config.h"

namespace nc::physics
{
    ColliderSystem::ColliderSystem(uint32_t maxColliders)
        : ComponentSystem<Collider>{maxColliders},
          m_dynamicSoA{maxColliders},
          m_staticTree{},
          m_maxColliders{maxColliders},
          m_locations{}
    {
    }

    ColliderSystem::~ColliderSystem() = default;

    ColliderTree* ColliderSystem::GetStaticTree()
    {
        return &m_staticTree;
    }

    ColliderSoA* ColliderSystem::GetDynamicSOA()
    {
        return &m_dynamicSoA;
    }

    Collider* ColliderSystem::AddStatic(EntityHandle handle, const ColliderInfo& info)
    {
        // consider max colliders?

        m_staticTree.Add(handle, info);

        // @todo add to m_locations?

        return ComponentSystem<Collider>::Add(handle, info);
    }

    Collider* ColliderSystem::AddDynamic(EntityHandle handle, const ColliderInfo& info)
    {
        // this doesn't make sense, max colliders should be split across both types
        if(m_dynamicSoA.nextFree >= m_maxColliders)
            return nullptr;


        uint32_t pos = [this]()
        {
            if(m_dynamicSoA.gaps.empty())
                return m_dynamicSoA.nextFree++;
            
            auto out = m_dynamicSoA.gaps.back();
            m_dynamicSoA.gaps.pop_back();
            return out;
        }();

        m_dynamicSoA.handles[pos] = static_cast<EntityHandle::Handle_t>(handle);
        m_dynamicSoA.transforms[pos] = &GetComponent<Transform>(handle)->GetTransformationMatrix();
        m_dynamicSoA.volumeData[pos] = ColliderSoA::CenterExtentPair{{info.offset.x, info.offset.y, info.offset.z}, {info.scale.x / 2.0f, info.scale.y / 2.0f, info.scale.z / 2.0f}};
        m_dynamicSoA.types[pos] = info.type;
        m_locations.emplace_back(handle, pos, &m_dynamicSoA);
        return ComponentSystem<Collider>::Add(handle, info);
    }

    Collider* ColliderSystem::Add(EntityHandle handle, const ColliderInfo& info)
    {
        if(GetEntity(handle)->IsStatic)
            return AddStatic(handle, info);
        
        return AddDynamic(handle, info);
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

        if(GetEntity(handle)->IsStatic)
            RemoveStatic(handle);
        else
            RemoveDynamic(pos->index);
        
        return true;
    }

    void ColliderSystem::RemoveStatic(EntityHandle handle)
    {
        m_staticTree.Remove(handle);
    }

    void ColliderSystem::RemoveDynamic(uint32_t index)
    {
        m_dynamicSoA.gaps.push_back(index);
        std::sort(m_dynamicSoA.gaps.begin(), m_dynamicSoA.gaps.end());
    }

    void ColliderSystem::Clear()
    {
        ComponentSystem::Clear();
        m_dynamicSoA.gaps.resize(0u);
        m_dynamicSoA.nextFree = 0u;
        m_staticTree.Clear();
        m_locations.resize(0u);
    }
} // namespace nc::physics