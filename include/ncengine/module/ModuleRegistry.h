/**
 * @file ModuleRegistry.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "Module.h"

#include "ncutility/NcError.h"

#include <algorithm>
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
            NC_ASSERT(module, "Attempt to register a null Module.");
            if (IsRegistered<T>())
            {
                Unregister<T>();
            }

            SetupId(*module);
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
        auto IsRegistered() const noexcept -> bool
        {
            return Get<T>() != nullptr;
        }

        /** @brief Check if a module matching an id is registered. */
        auto IsRegistered(size_t id) const noexcept -> bool
        {
            return std::ranges::contains(m_modules, id, [](const auto& module) { return module->Id(); });
        }

        /** @brief Get a pointer to a module or nullptr if it is unregistered. */
        template<std::derived_from<Module> T>
        auto Get() const noexcept -> T*
        {
            return m_typedModulePointer<T>;
        }

        /** @brief Get a pointer to a module by id or nullptr if it is unregistered. */
        auto Get(size_t id) const noexcept -> Module*
        {
            auto pos = std::ranges::find_if(m_modules, [id](auto& module) { return module->Id() == id; });
            return pos != std::ranges::end(m_modules) ? pos->get() : nullptr;
        }

        /*** @brief Get the collection of all registered modules. */
        auto GetAllModules() noexcept -> const std::vector<std::unique_ptr<Module>>&
        {
            return m_modules;
        }

    private:
        std::vector<std::unique_ptr<Module>> m_modules;
        size_t m_nextModuleId = std::numeric_limits<size_t>::max();

        template<std::derived_from<Module> T>
        inline static T* m_typedModulePointer = nullptr;

        void SetupId(Module& module)
        {
            if (module.Id() == 0ull)
                module.SetId(m_nextModuleId--);

            const auto id = module.Id();
            if (IsRegistered(id))
                throw NcError(fmt::format("Module id '{}' is already in use.", id));
        }
};
} // namespace nc
