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

Registry::Registry(size_t maxEntities)
    : m_impl{maxEntities},
      m_ecs{m_impl},
      m_maxEntities{maxEntities}
{
    g_registry = this;
}
} // namespace nc
