#include "ColliderSystem.h"
#include "config/Config.h"
#include "Entity.h"

namespace nc::ecs
{
    ColliderSystem::ColliderSystem(uint32_t maxDynamic,
                                   uint32_t maxStatic,
                                   uint32_t octreeDensityThreshold,
                                   float octreeMinimumExtent,
                                   float worldspaceExtent)
        : m_dynamicSoA{maxDynamic},
          m_staticTree{maxStatic, octreeDensityThreshold, octreeMinimumExtent, worldspaceExtent},
          m_hullColliderManager{}
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
        auto entity = collider.GetParentEntity();

        if(EntityUtils::IsStatic(entity))
            m_staticTree.Add(entity, collider.GetInfo());
        else
        {
            const auto& info = collider.GetInfo();

            m_dynamicSoA.Add
            (
                static_cast<EntityTraits::underlying_type>(entity),
                DirectX::XMMATRIX{},
                physics::CreateBoundingVolume(info),
                collider.GetType()
            );
        }
    }

    void ColliderSystem::Remove(Entity entity)
    {
        if(EntityUtils::IsStatic(entity))
            m_staticTree.Remove(entity);
        else
            m_dynamicSoA.Remove(static_cast<EntityTraits::underlying_type>(entity));
    }

    void ColliderSystem::Clear()
    {
        m_dynamicSoA.Clear();
        m_staticTree.Clear();
    }
} // namespace nc::ecs