#pragma once
#include "SpawnPropertyGenerator.h"
#include "shared/Prefabs.h"
#include "shared/ConstantTranslation.h"
#include "shared/ConstantRotation.h"

#include <functional>

namespace nc::sample
{
    /** A prefab spawner configurable with a SpawnBehavior. If provided, extension
        will be applied to each handle after creation. */
    class Spawner : public Component
    {
        public:
            using SpawnExtension = std::function<void(EntityHandle)>;

            Spawner(EntityHandle handle, prefab::Resource resource, SpawnBehavior behavior, SpawnExtension extension = nullptr);
            void FrameUpdate(float) override;
            void StageSpawn(unsigned count = 1u);
            void Spawn(unsigned count = 1u);
            void StageDestroy(unsigned count = 1u);
            void Destroy(unsigned count = 1u);
            void SetPrefab(prefab::Resource resource);
            const std::vector<EntityHandle>& GetHandles() const;
            int GetObjectCount() const;
        
        private:
            SpawnExtension m_extension;
            std::vector<EntityHandle> m_entities;
            SpawnPropertyGenerator m_generator;
            prefab::Resource m_resource;
            bool m_applyConstantVelocity;
            bool m_applyConstantRotation;
            bool m_spawnStaticEntities;
            size_t m_stagedAdditions;
            size_t m_stagedDeletions;
    };

    inline Spawner::Spawner(EntityHandle handle, prefab::Resource resource, SpawnBehavior behavior, SpawnExtension extension)
        : Component{handle},
          m_extension{extension},
          m_entities{},
          m_generator{behavior},
          m_resource{resource},
          m_applyConstantVelocity{Vector3::Zero() != behavior.velocityRandomRange},
          m_applyConstantRotation{Vector3::Zero() != behavior.rotationAxisRandomRange &&
                                  0.0f != behavior.thetaRandomRange},
          m_spawnStaticEntities{behavior.spawnAsStaticEntity &&
                                !m_applyConstantRotation &&
                                !m_applyConstantRotation},
          m_stagedAdditions{0u},
          m_stagedDeletions{0u}
    {
    }

    inline void Spawner::FrameUpdate(float)
    {
        // Additions/Deletions are delayed until FrameUpdate because Spawn/Destroy are
        // callbacks from ui events, and modifying state in the middle of rendering can
        // cause problems.
        if(m_stagedAdditions)
        {
            Spawn(m_stagedAdditions);
            m_stagedAdditions = 0u;
        }

        if(m_stagedDeletions)
        {
            Destroy(m_stagedDeletions);
            m_stagedDeletions = 0u;
        }
    }

    inline void Spawner::StageSpawn(unsigned count)
    {
        m_stagedAdditions = count;
    }

    inline void Spawner::Spawn(unsigned count)
    {
        std::generate_n(std::back_inserter(m_entities), count, [this]()
        {
            auto handle = prefab::Create(m_resource,
            {
                .position = m_generator.Position(),
                .rotation = Quaternion::FromEulerAngles(m_generator.Rotation()),
                .flags = m_spawnStaticEntities
            });

            if(m_applyConstantVelocity)
                AddComponent<ConstantTranslation>(handle, m_generator.Velocity());
            if(m_applyConstantRotation)
                AddComponent<ConstantRotation>(handle, m_generator.RotationAxis(), m_generator.Theta());
            if(m_extension)
                m_extension(handle);
            return handle;
        });
    }

    inline void Spawner::StageDestroy(unsigned count)
    {
        m_stagedDeletions = count;
    }

    inline void Spawner::Destroy(unsigned count)
    {
        while(!m_entities.empty() && count--)
        {
            DestroyEntity(m_entities.back());
            m_entities.pop_back();
        }
    }

    inline void Spawner::SetPrefab(prefab::Resource resource)
    {
        m_resource = resource;
    }

    inline const std::vector<EntityHandle>& Spawner::GetHandles() const
    {
        return m_entities;
    }
    
    inline int Spawner::GetObjectCount() const
    {
        return m_entities.size();
    }
}