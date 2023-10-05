#pragma once
#include "SpawnPropertyGenerator.h"
#include "shared/Prefabs.h"
#include "shared/FreeComponents.h"

#include "ncengine/math/Random.h"

#include <functional>

namespace nc::sample
{
/** A prefab spawner configurable with a SpawnBehavior. If provided, extension
    will be applied to each handle after creation. */
class Spawner : public FreeComponent
{
    public:
        using SpawnExtension = std::function<void(Entity)>;

        Spawner(Entity entity,
                Random* random,
                prefab::Resource resource,
                SpawnBehavior behavior,
                SpawnExtension extension = nullptr);
        void Run(Entity, Registry*, float);
        void StageSpawn(unsigned count = 1u);
        void Spawn(Registry* registry, unsigned count = 1u);
        void StageDestroy(unsigned count = 1u);
        void Destroy(Registry* registry, unsigned count = 1u);
        void SetPrefab(prefab::Resource resource);
        const std::vector<Entity>& GetHandles() const;
        int GetObjectCount() const;
    
    private:
        SpawnExtension m_extension;
        std::vector<Entity> m_entities;
        SpawnPropertyGenerator m_generator;
        prefab::Resource m_resource;
        bool m_applyConstantVelocity;
        bool m_applyConstantRotation;
        Entity::layer_type m_layer;
        Entity::flags_type m_flags;
        unsigned m_stagedAdditions;
        unsigned m_stagedDeletions;
};

inline Spawner::Spawner(Entity entity,
                        Random* random,
                        prefab::Resource resource,
                        SpawnBehavior behavior,
                        SpawnExtension extension)
    : FreeComponent{entity},
      m_extension{extension},
      m_entities{},
      m_generator{behavior, random},
      m_resource{resource},
      m_applyConstantVelocity{Vector3::Zero() != behavior.minVelocity &&
                              Vector3::Zero() != behavior.maxVelocity},
      m_applyConstantRotation{Vector3::Zero() != behavior.rotationAxis ||
                              0.0f != behavior.rotationTheta},
      m_layer{behavior.layer},
      m_flags{behavior.flags},
      m_stagedAdditions{0u},
      m_stagedDeletions{0u}
{
}

inline void Spawner::Run(Entity, Registry* registry, float)
{
    // Additions/Deletions are delayed until FrameUpdate because Spawn/Destroy are
    // callbacks from ui events, and modifying state in the middle of rendering can
    // cause problems.
    if(m_stagedAdditions)
    {
        Spawn(registry, m_stagedAdditions);
        m_stagedAdditions = 0u;
    }

    if(m_stagedDeletions)
    {
        Destroy(registry, m_stagedDeletions);
        m_stagedDeletions = 0u;
    }
}

inline void Spawner::StageSpawn(unsigned count)
{
    m_stagedAdditions = count;
}

inline void Spawner::Spawn(Registry* registry, unsigned count)
{
    std::generate_n(std::back_inserter(m_entities), count, [this, registry]()
    {
        auto handle = prefab::Create(registry, m_resource,
        {
            .position = m_generator.Position(),
            .rotation = Quaternion::FromEulerAngles(m_generator.Rotation()),
            .layer = m_layer,
            .flags = m_flags
        });

        if(m_applyConstantVelocity)
            registry->Add<ConstantTranslation>(handle, m_generator.Velocity());
        if(m_applyConstantRotation)
            registry->Add<ConstantRotation>(handle, m_generator.RotationAxis(), m_generator.Theta());

        registry->Add<FrameLogic>(handle, [](Entity self, Registry* reg, float dt)
        {
            if(auto* translation = reg->Get<ConstantTranslation>(self))
                translation->Run(self, reg, dt);
            if(auto* rotation = reg->Get<ConstantRotation>(self))
                rotation->Run(self, reg, dt);
        });

        if(m_extension)
            m_extension(handle);
        return handle;
    });
}

inline void Spawner::StageDestroy(unsigned count)
{
    m_stagedDeletions = count;
}

inline void Spawner::Destroy(Registry* registry, unsigned count)
{
    while(!m_entities.empty() && count--)
    {
        registry->Remove<Entity>(m_entities.back());
        m_entities.pop_back();
    }
}

inline void Spawner::SetPrefab(prefab::Resource resource)
{
    m_resource = resource;
}

inline const std::vector<Entity>& Spawner::GetHandles() const
{
    return m_entities;
}

inline int Spawner::GetObjectCount() const
{
    return static_cast<int>(m_entities.size());
}
}
