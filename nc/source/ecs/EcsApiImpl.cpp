#include "Ecs.h"
#include "ecs/EntityComponentSystem.h"

namespace nc
{
    namespace internal
    {
        registry_type* g_registry = nullptr;

        void SetActiveRegistry(registry_type* registry)
        {
            g_registry = registry;
        }
    }

    auto ActiveRegistry() -> registry_type*
    {
        return internal::g_registry;
    }
}