#include "ecs/Registry.h"
#include "audio/AudioSource.h"
#include "ecs/Logic.h"
#include "ecs/Tag.h"
#include "ecs/Transform.h"
#include "ecs/detail/FreeComponentGroup.h"
#include "graphics/Camera.h"
#include "graphics/MeshRenderer.h"
#include "graphics/ParticleEmitter.h"
#include "graphics/PointLight.h"
#include "network/NetworkDispatcher.h"
#include "physics/Collider.h"
#include "physics/ConcaveCollider.h"
#include "physics/PhysicsBody.h"

/** @todo This needs to go away. */
namespace
{
nc::Registry* g_registry = nullptr;
}

namespace nc
{
auto ActiveRegistry() -> Registry*
{
    return g_registry;
}

Registry::Registry(size_t maxEntities)
    : m_registeredStorage{},
      m_entities{},
      m_maxEntities{maxEntities}
{
    g_registry = this;

    RegisterComponentType<CollisionLogic>();
    RegisterComponentType<FrameLogic>();
    RegisterComponentType<FixedLogic>();
    RegisterComponentType<Tag>();
    RegisterComponentType<Transform>();
    RegisterComponentType<audio::AudioSource>();
    RegisterComponentType<ecs::detail::FreeComponentGroup>();
    RegisterComponentType<graphics::MeshRenderer>();
    RegisterComponentType<graphics::ParticleEmitter>();
    RegisterComponentType<graphics::PointLight>();
    RegisterComponentType<net::NetworkDispatcher>();
    RegisterComponentType<physics::Collider>();
    RegisterComponentType<physics::ConcaveCollider>();
    RegisterComponentType<physics::PhysicsBody>();
}

void Registry::RemoveEntityWithoutNotifyingParent(Entity entity)
{
    NC_ASSERT(m_entities.Contains(entity), "Bad Entity");
    auto* transform = Get<Transform>(entity);

    for(auto child : transform->Children())
        RemoveEntityWithoutNotifyingParent(child);

    m_entities.Remove(entity);
}

void Registry::Clear()
{
    m_entities.Clear();

    for(auto& storage : m_registeredStorage)
    {
        storage->Clear();
    }
}
} // namespace nc
