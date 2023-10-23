/**
 * @file ModuleRegistry.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include "Module.h"

#include <concepts>
#include <memory>

namespace nc
{
class ModuleRegistry
{
    public:
        /** @brief Add a new module to the registry. Overwrites any existing module of the same type. */
        template<std::derived_from<Module> T>
        void Register(std::unique_ptr<T> module)
        {
            if (IsRegistered<T>())
            {
                Unregister<T>();
            }

            m_typedModulePointer<T> = module.get();
            m_modules.push_back(std::move(module));
        }

        /** @brief Remove a module from the registry. Does nothing if no matching module is found. */
        template<std::derived_from<Module> T>
        void Unregister()
        {
            const auto* target = m_typedModulePointer<T>;
            std::erase_if(m_modules, [target](const auto& module)
            {
                return module.get() == target;
            });

            m_typedModulePointer<T> = nullptr;
        }

        /** @brief Check if a module matching a type is registered. */
        template<std::derived_from<Module> T>
        bool IsRegistered() const noexcept
        {
            return Get<T>() != nullptr;
        }

        /** @brief Get a pointer to a module or nullptr if it is unregistered. */
        template<std::derived_from<Module> T>
        auto Get() const noexcept -> T*
        {
            return m_typedModulePointer<T>;
        }

        /*** @brief Get the collection of all registered modules. */
        auto GetAllModules() noexcept -> const std::vector<std::unique_ptr<Module>>&
        {
            return m_modules;
        }

    private:
        std::vector<std::unique_ptr<Module>> m_modules;

        template<std::derived_from<Module> T>
        inline static T* m_typedModulePointer = nullptr;
};
} // namespace nc
