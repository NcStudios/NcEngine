#pragma once
#include "SpawnPropertyGenerator.h"
#include "shared/Prefabs.h"

#include "ncengine/math/Random.h"

#include <functional>

namespace nc::sample
{
/** A prefab spawner configurable with a SpawnBehavior. If provided, extension
    will be applied to each handle after creation, and the prePostSpawn
    is called before and after spawning all objects.
*/
class Spawner : public FreeComponent
{
    public:
        using SpawnExtension = std::function<void(Entity)>;
        using PrePostOp = std::function<void(bool isPreSpawn, unsigned count)>;

        Spawner(Entity entity,
                Random* random,
                SpawnBehavior behavior,
                SpawnExtension extension = nullptr,
                PrePostOp prePostSpawn = nullptr);
        void Run(Entity, Registry*, float);
        void StageSpawn(unsigned count = 1u);
        void Spawn(Registry* registry, unsigned count = 1u);
        void StageDestroy(unsigned count = 1u);
        void Destroy(Registry* registry, unsigned count = 1u);
        const std::vector<Entity>& GetHandles() const;
        int GetObjectCount() const;

    private:
        SpawnExtension m_extension;
        PrePostOp m_prePostSpawn;
        std::vector<Entity> m_entities;
        SpawnPropertyGenerator m_generator;
        Entity::layer_type m_layer;
        Entity::flags_type m_flags;
        unsigned m_stagedAdditions;
        unsigned m_stagedDeletions;
};

inline Spawner::Spawner(Entity entity,
                        Random* random,
                        SpawnBehavior behavior,
                        SpawnExtension extension,
                        PrePostOp prePostSpawn)
    : FreeComponent{entity},
      m_extension{std::move(extension)},
      m_prePostSpawn{std::move(prePostSpawn)},
      m_entities{},
      m_generator{behavior, random},
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
    if (m_prePostSpawn)
        m_prePostSpawn(true, count);

    std::generate_n(std::back_inserter(m_entities), count, [this, registry]()
    {
        const auto handle = registry->Add<Entity>({
            .position = m_generator.Position(),
            .rotation = Quaternion::FromEulerAngles(m_generator.Rotation()),
            .layer = m_layer,
            .flags = m_flags
        });

        if (m_extension)
            m_extension(handle);

        return handle;
    });

    if (m_prePostSpawn)
        m_prePostSpawn(false, count);
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

inline const std::vector<Entity>& Spawner::GetHandles() const
{
    return m_entities;
}

inline int Spawner::GetObjectCount() const
{
    return static_cast<int>(m_entities.size());
}
}
