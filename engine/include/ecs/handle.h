#pragma once

#include "component/Component.h"

namespace nc
{
    class Registry;

    /**
     * @brief Wrapper around an entity and its owning registry.
     * 
     * @tparam RegistryType Always specified as Registry. Templating is
     * an implementation detail.
     */
    template<class RegistryType = Registry>
    class handle_base
    {
        public:
            /**
             * @brief Construct a new handle object.
             * @param reg A pointer to a valid registry.
             * @param ent An entity owned by reg.
             */
            explicit handle_base(RegistryType* reg, Entity ent)
                : m_registry{reg}, m_entity{ent} {}

            /**
             * @brief Add a component to the entity.
             * @tparam T The component type.
             * @tparam Args The type(s) of args...
             * @param args Arguments to construct the component with.
             * @return T* A pointer to the added component.
             */
            template<std::derived_from<ComponentBase> T, class... Args>
            auto add(Args&&... args) -> T*
            {
                return m_registry->template Add<T, Args...>(m_entity, std::forward<Args>(args)...);
            }

            /**
             * @brief Remove a component from the entity.
             * @tparam T The component type.
             */
            template<std::derived_from<ComponentBase> T>
            void remove()
            {
                m_registry->template Remove<T>(m_entity);
            }

            /**
             * @brief Check if the entity has a given component type.
             * @tparam T The component type.
             * @return bool Whether or not the component exists.
             */
            template<std::derived_from<ComponentBase> T>
            bool contains() const
            {
                return m_registry->template Contains<T>(m_entity);
            }

            /**
             * @brief Get a pointer to a compontent attached to the entity.
             * @tparam T The component type.
             * @return T* A pointer to the component, if found, or nullptr.
             */
            template<std::derived_from<ComponentBase> T>
            auto get() -> T*
            {
                return m_registry->template Get<T>(m_entity);
            }

            /**
             * @brief Get a pointer to a compontent attached to the entity.
             * @tparam T The component type.
             * @return const T* A pointer to the const-qualified component, if found, or nullptr.
             */
            template<std::derived_from<ComponentBase> T>
            auto get() const -> const T*
            {
                return m_registry->template Get<T>(m_entity);
            }

            /** @brief Check validity of the entity. */
            bool valid() const
            {
                return m_entity.Valid();
            }

            /** @brief Get the bound entity. */
            auto entity() const -> Entity
            {
                return m_entity;
            }

            /** @brief Get a pointer to the registry owning the entity. */
            auto registry() -> RegistryType*
            {
                return m_registry;
            }

            /** @brief Checks equality of handles by comparing their entities. */
            friend bool operator==(const handle_base<RegistryType>& lhs, const handle_base<RegistryType>& rhs)
            {
                return lhs.m_entity == rhs.m_entity;
            }

            /** @brief Checks inequality of handles by comparing their entities. */
            friend bool operator!=(const handle_base<RegistryType>& lhs, const handle_base<RegistryType>& rhs)
            {
                return !(lhs == rhs);
            }

        private:
            RegistryType* m_registry;
            Entity m_entity;
    };

    /** @brief Alias for using handle_base. */
    using handle = handle_base<Registry>;
}