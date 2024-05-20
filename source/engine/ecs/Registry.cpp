#include "ecs/Registry.h"

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

void SetActiveRegistry(Registry* registry)
{
    g_registry = registry;
}

Registry::Registry(ecs::ComponentRegistry& impl)
    : m_impl{&impl},
      m_ecs{impl}
{
}
} // namespace nc
