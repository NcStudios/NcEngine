#include "ColliderSystem.h"
#include "config/Config.h"

namespace nc::ecs
{
    ColliderSystem::ColliderSystem(uint32_t maxDynamic,
                                   uint32_t maxStatic,
                                   uint32_t octreeDensityThreshold,
                                   float octreeMinimumExtent,
                                   float worldspaceExtent)
        : m_componentSystem{maxDynamic + maxStatic},
          m_dynamicSoA{maxDynamic},
          m_staticTree{maxStatic, octreeDensityThreshold, octreeMinimumExtent, worldspaceExtent}
    {
    }

    ColliderSystem::~ColliderSystem() = default;

    ecs::ComponentSystem<Collider>* ColliderSystem::GetComponentSystem()
    {
        return &m_componentSystem;
    }

    ColliderTree* ColliderSystem::GetStaticTree()
    {
        return &m_staticTree;
    }

    ColliderSystem::DynamicColliderSoA* ColliderSystem::GetDynamicSoA()
    {
        return &m_dynamicSoA;
    }

    Collider* ColliderSystem::Add(EntityHandle handle, const ColliderInfo& info)
    {
        if(GetEntity(handle)->IsStatic)
            m_staticTree.Add(handle, info);
        else
        {
            m_dynamicSoA.Add
            (
                static_cast<EntityHandle::Handle_t>(handle),
                &GetComponent<Transform>(handle)->GetTransformationMatrix(),
                physics::GetVolumePropertiesFromColliderInfo(info),
                info.type
            );
        }

        return m_componentSystem.Add(handle, info);
    }

    bool ColliderSystem::Remove(EntityHandle handle, bool isStatic)
    {
        if(!m_componentSystem.Remove(handle))
            return false;

        if(isStatic)
            m_staticTree.Remove(handle);
        else
            m_dynamicSoA.Remove(static_cast<EntityHandle::Handle_t>(handle));

        return true;
    }

    void ColliderSystem::Clear()
    {
        m_componentSystem.Clear();
        m_dynamicSoA.Clear();
        m_staticTree.Clear();
    }

    bool ColliderSystem::Contains(EntityHandle handle) const
    {
        return m_componentSystem.Contains(handle);
    }

    Collider* ColliderSystem::GetPointerTo(EntityHandle handle)
    {
        return m_componentSystem.GetPointerTo(handle);
    }

    std::span<Collider*> ColliderSystem::GetComponents()
    {
        return m_componentSystem.GetComponents();
    }
} // namespace nc::ecs