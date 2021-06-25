#pragma once

#include "shared/Prefabs.h"
#include "WasdController.h"
#include "CollisionLogger.h"

namespace nc::sample
{
    class PrefabSelector : public AutoComponent
    {
        public:
            PrefabSelector(Entity entity, registry_type* registry);
            
            void FrameUpdate(float) override;
            void Select(ColliderType type);

        private:
            registry_type* m_registry;
            Entity m_currentObject;
            ColliderType m_typeToSpawn;
            bool m_doSpawn;
    };

    PrefabSelector::PrefabSelector(Entity entity, registry_type* registry)
        : AutoComponent{entity},
          m_registry{registry},
          m_currentObject{Entity::Null()},
          m_typeToSpawn{ColliderType::Capsule},
          m_doSpawn{true}
    {
    }

    void PrefabSelector::FrameUpdate(float)
    {
        if(!m_doSpawn)
            return;
        
        m_doSpawn = false;

        if(m_currentObject.Valid())
        {
            m_registry->Remove<Collider>(m_currentObject);
            m_registry->Remove<Entity>(m_currentObject);
        }

        switch(m_typeToSpawn)
        {
            case ColliderType::Box:
            {
                m_currentObject = prefab::Create(m_registry, prefab::Resource::CubeBlue, {.tag = "Movable Object"});
                m_registry->Add<Collider>(m_currentObject, BoxProperties{});
                break;
            }
            case ColliderType::Sphere:
            {
                m_currentObject = prefab::Create(m_registry, prefab::Resource::SphereBlue, {.tag = "Movable Object"});
                m_registry->Add<Collider>(m_currentObject, SphereProperties{});
                break;
            }
            case ColliderType::Capsule:
            {
                m_currentObject = prefab::Create(m_registry, prefab::Resource::CapsuleBlue, {.tag = "Movable Object"});
                m_registry->Add<Collider>(m_currentObject, CapsuleProperties{});
                break;
            }
            case ColliderType::Hull:
            {
                m_currentObject = prefab::Create(m_registry, prefab::Resource::DiscBlue, {.tag = "Movable Object"});
                m_registry->Add<Collider>(m_currentObject, HullProperties{.assetPath = "project/assets/mesh_colliders/coin.nca"});
                break;
            }
        }

        m_registry->Add<WasdController>(m_currentObject, m_registry, 2.0f);
        m_registry->Add<CollisionLogger>(m_currentObject, m_registry);
    }

    void PrefabSelector::Select(ColliderType type)
    {
        m_typeToSpawn = type;
        m_doSpawn = true;
    }
}