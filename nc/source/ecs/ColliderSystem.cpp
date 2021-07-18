#include "ColliderSystem.h"
#include "config/Config.h"
#include "Entity.h"

namespace nc::ecs
{
    ColliderSystem::ColliderSystem(registry_type* registry,
                                   uint32_t maxDynamic,
                                   uint32_t maxStatic,
                                   uint32_t octreeDensityThreshold,
                                   float octreeMinimumExtent,
                                   float worldspaceExtent)
        : m_dynamicSoA{maxDynamic},
          m_staticTree{maxStatic, octreeDensityThreshold, octreeMinimumExtent, worldspaceExtent},
          m_hullColliderManager{}
    {
        registry->RegisterOnAddCallback<Collider>
        (
            [this](const Collider& collider) { this->Add(collider); }
        );

        registry->RegisterOnRemoveCallback<Collider>
        (
            [this](Entity entity) { this->Remove(entity); }
        );
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
                collider.GetType(),
                info.isTrigger
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