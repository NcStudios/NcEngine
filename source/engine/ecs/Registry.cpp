#include "ecs/Registry.h"
#include "ecs/Tag.h"
#include "ecs/Transform.h"
#include "ecs/detail/FreeComponentGroup.h"

/** @todo #433 This needs to go away. */
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
    // These component types are directly used by the registry
    RegisterComponentType<Tag>();
    RegisterComponentType<Transform>();
    RegisterComponentType<ecs::detail::FreeComponentGroup>();
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
