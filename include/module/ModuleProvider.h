#pragma once

#include "ModuleRegistry.h"

namespace nc
{
class ModuleProvider
{
    public:
        ModuleProvider(ModuleRegistry* registry)
            : m_registry{registry}
        {
        }

        template<std::derived_from<Module> T>
        auto Get() -> T*
        {
            return m_registry->Get<T>();
        }

    private:
        ModuleRegistry* m_registry;
};
}