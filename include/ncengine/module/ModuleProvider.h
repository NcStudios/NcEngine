/**
 * @file ModuleProvider.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ModuleRegistry.h"

namespace nc
{
/** @brief Provides access to registered Modules. */
class ModuleProvider
{
    public:
        ModuleProvider(ModuleRegistry* registry)
            : m_registry{registry}
        {
        }

        /** @brief Get a pointer to a Module that was registered with the given type. */
        template<std::derived_from<Module> T>
        auto Get() -> T*
        {
            return m_registry->Get<T>();
        }

        /** @brief Get a pointer to the Module that was registered with the given id. */
        template<std::derived_from<Module> T>
        auto Get(size_t id) -> Module*
        {
            return m_registry->Get(id);
        }

        /** @brief Apply a function to all registered Modules. */
        template<std::invocable<Module*> F>
        void ForEachModule(F func)
        {
            for (auto& module : m_registry->GetAllModules())
            {
                std::invoke(func, module.get());
            }
        }

    private:
        ModuleRegistry* m_registry;
};
} // namespace nc
