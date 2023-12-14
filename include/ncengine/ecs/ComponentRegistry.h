/**
 * @file ComponentRegistry.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include "ncengine/ecs/ComponentPool.h"
#include "ncengine/ecs/EntityPool.h"

namespace nc::ecs
{
/**
 * @brief Core collection of data pools and registration information game types.
 * 
 * The ComponentRegistry houses all entity and component data and provides a low-level interface over game state. An
 * instance is created an bootstrapped with engine types during NcEngine initialization. This instance remains valid
 * and pointer stable until the engine is destroyed. Capacity parameters are taken from the config::Config object used
 * to initialize the engine.
 * 
 * Custom component types may be registered at any time to create pools for managing component instances. For types
 * satisfying PooledComponent, registration is required prior to working with that type through any ecs operation.
 * Registration is not necessary for any types deriving from FreeComponent.
 */
class ComponentRegistry : public StableAddress
{
    public:
        /**
         * @brief Construct a ComponentRegistry.
         * @param entityCapacity Max concurrent entity instances.
         * @note Only one ComponentRegistry instance can exist at a time. An instance is created automatically during
         *       initialization of the NcEngine instance.
         */
        explicit ComponentRegistry(size_t entityCapacity)
            : m_maxEntities{entityCapacity}
        {
            NC_ASSERT(!s_init, "There may only be one ComponentRegistry instance.");
            s_init = true;
        }

        ~ComponentRegistry() noexcept
        {
            std::ranges::for_each(m_refs, [](auto&& p) { *p = nullptr; });
            s_init = false;
        }

        /**
         * @brief Register a component type.
         * @param capacity Max concurrent instances - must be <= the registry's entity capacity.
         * @param handler Optional callbacks for handling generic component operations.
         */
        template<PooledComponent T>
        void RegisterType(size_t capacity, ComponentHandler<T> handler = {})
        {
            NC_ASSERT(capacity <= m_maxEntities, "Component capacity cannot exceed entity capapcity.");
            NC_ASSERT(!s_typedPool<T>, "Type already registered");
            SetupStorage<T>(std::make_unique<ComponentPool<T>>(capacity, std::move(handler)));
        }

        /** @brief Check if a type is registered. */
        template<PooledComponent T>
        auto IsTypeRegistered() const noexcept -> bool
        {
            return s_typedPool<T> != nullptr;
        }

        /** @brief Get the pool for a registered component type. */
        template<PooledComponent T>
        auto GetPool() -> ComponentPool<T>&
        {
            NC_ASSERT(s_typedPool<T>, "Attempt to access an unregistered type.");
            return *s_typedPool<T>;
        }

        /** @brief Get the pool for a registered component type. */
        template<PooledComponent T>
        auto GetPool() const -> const ComponentPool<T>&
        {
            NC_ASSERT(s_typedPool<T>, "Attempt to access an unregistered type.");
            return *s_typedPool<T>;
        }

        /** @brief Get the entity pool. */
        template<std::same_as<Entity> T>
        auto GetPool() -> EntityPool& { return m_entities; }

        /** @brief Get the entity pool. */
        template<std::same_as<Entity> T>
        auto GetPool() const -> const EntityPool& { return m_entities; }

        /** @brief Get a view of all registered component pools as ComponentPoolBase*. */
        auto GetComponentPools()
        {
            return m_pools | std::views::transform(&std::unique_ptr<ComponentPoolBase>::get);
        }

        /** @brief Get the maximum number of concurrent entities supported. */
        auto GetMaxEntities() const noexcept { return m_maxEntities; }

        /** @brief Merge staged components into their pools and finalize any entity removals. */
        void CommitPendingChanges()
        {
            const auto removed = m_entities.RecycleDeadEntities();
            std::ranges::for_each(m_pools, [&removed](auto&& p) { p->CommitStagedComponents(removed); });
        }

        /** @brief Destroy all non-persistent entities and components. */
        void ClearSceneData()
        {
            std::ranges::for_each(m_pools, [](auto&& p) { p->ClearNonPersistent(); });
            m_entities.ClearNonPersistent();
        }

        /** @brief Destroy all entities and components. */
        void Clear()
        {
            std::ranges::for_each(m_pools, [](auto&& p) { p->Clear(); });
            m_entities.Clear();
        }

    private:
        std::vector<std::unique_ptr<ComponentPoolBase>> m_pools;
        EntityPool m_entities;
        std::vector<void**> m_refs;
        size_t m_maxEntities;

        template<PooledComponent T>
        inline static ComponentPool<T>* s_typedPool = nullptr;

        inline static bool s_init = false;

        template<PooledComponent T>
        void SetupStorage(std::unique_ptr<ComponentPool<T>> pool) noexcept
        {
            s_typedPool<T> = pool.get();
            m_refs.emplace_back(reinterpret_cast<void**>(&s_typedPool<T>));
            m_pools.push_back(std::move(pool));
        }
};
} // namespace nc::ecs
