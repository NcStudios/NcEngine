#include "ColliderSystem.h"
#include "config/Config.h"
#include "entity/HandleUtils.h"

namespace nc::ecs
{
    ColliderSystem::ColliderSystem(uint32_t maxDynamic,
                                   uint32_t maxStatic,
                                   uint32_t octreeDensityThreshold,
                                   float octreeMinimumExtent,
                                   float worldspaceExtent)
        : m_dynamicSoA{maxDynamic},
          m_staticTree{maxStatic, octreeDensityThreshold, octreeMinimumExtent, worldspaceExtent}
    {
    }

    ColliderSystem::~ColliderSystem() = default;

    ColliderTree* ColliderSystem::GetStaticTree()
    {
        return &m_staticTree;
    }

    ColliderSystem::DynamicColliderSoA* ColliderSystem::GetDynamicSoA()
    {
        return &m_dynamicSoA;
    }
    
    void ColliderSystem::Add(const Collider& collider)
    {
        auto handle = collider.GetParentHandle();

        if(HandleUtils::IsStatic(handle))
            m_staticTree.Add(handle, collider.GetInfo());
        else
        {
            m_dynamicSoA.Add
            (
                static_cast<HandleTraits::handle_type>(handle),
                DirectX::XMMATRIX{},
                physics::GetVolumePropertiesFromColliderInfo(collider.GetInfo()),
                collider.GetType()
            );
        }
    }

    void ColliderSystem::Remove(EntityHandle handle)
    {
        if(HandleUtils::IsStatic(handle))
            m_staticTree.Remove(handle);
        else
            m_dynamicSoA.Remove(static_cast<HandleTraits::handle_type>(handle));
    }

    void ColliderSystem::Clear()
    {
        m_dynamicSoA.Clear();
        m_staticTree.Clear();
    }
} // namespace nc::ecs