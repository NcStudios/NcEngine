#include "ColliderSystem.h"
#include "config/Config.h"
#include "Entity.h"

namespace nc::ecs
{
    VolumeProperties GetVolumePropertiesFromColliderInfo(const ColliderInfo& info)
    {
        // for mesh should get support in x/y/z for maxExtent

        float maxExtent = 0.0f;
        switch(info.type)
        {
            case ColliderType::Box:
            {
                maxExtent = Magnitude(info.scale / 2.0f);
                //maxExtent = Magnitude(info.scale) / 2.0f;
                break;
            }
            case ColliderType::Sphere:
            {
                maxExtent = info.scale.x / 2.0f;
                break;
            }
            case ColliderType::Mesh: // fix
            {
                maxExtent = 0.5f;
                break;
            }
        }

        return VolumeProperties
        {
            info.offset,
            info.scale / 2.0f,
            maxExtent
        };
    }

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
                CreateBoundingVolume(info.type, info.offset, info.scale),
                GetVolumePropertiesFromColliderInfo(info),
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