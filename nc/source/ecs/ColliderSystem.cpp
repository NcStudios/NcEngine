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
        : m_registry{registry},
          m_dynamicSoA{maxDynamic},
          m_staticTree{maxStatic, octreeDensityThreshold, octreeMinimumExtent, worldspaceExtent},
          m_hullColliderManager{}
    {
        m_registry->RegisterOnAddCallback<Collider>
        (
            [this](const Collider& collider) { this->Add(collider); }
        );

        m_registry->RegisterOnRemoveCallback<Collider>
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

        #ifdef NC_EDITOR_ENABLED
        AddDebugWidget(collider);
        #endif

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
        #ifdef NC_EDITOR_ENABLED
        m_registry->Remove<vulkan::DebugWidget>(entity);
        #endif

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

    #ifdef NC_EDITOR_ENABLED
    void ColliderSystem::AddDebugWidget(const Collider& collider)
    {
        auto entity = collider.GetParentEntity();

        vulkan::WidgetShape shape;
        switch (collider.GetType())
        {
            case ColliderType::Box:
            {
                shape = vulkan::WidgetShape::Cube;
                break;
            }
            case ColliderType::Sphere:
            {
                shape = vulkan::WidgetShape::Sphere;
                break;
            }
            case ColliderType::Capsule:
            {
                shape = vulkan::WidgetShape::Capsule;
                break;
            }
        }

        m_registry->Add<vulkan::DebugWidget>(entity, shape);
    }
    #endif

} // namespace nc::ecs