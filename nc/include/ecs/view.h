#pragma once

#include "detail/view_iterator.h"

namespace nc
{
    /**
     * @brief View over multiple components.
     * 
     * Represents a range of entities containing each of the specified components. Iteration
     * invalidation rules are the same as the registry.
     */
    template<PooledComponent... Ts>
    class view
    {
        public:
            using iterator = detail::multi_view_iterator<Ts...>;
            using storage = detail::PerComponentStorageBase;

            view(Registry* registry)
                : m_registry{registry},
                  m_viewBasis{find_basis_storage()}
            {
            }

            auto begin() noexcept -> iterator
            {
                auto& pool = m_viewBasis->EntityPool();
                return iterator{pool.begin(), pool.end(), m_registry};
            }

            auto end() noexcept -> iterator
            {
                auto& pool = m_viewBasis->EntityPool();
                return iterator{pool.end(), pool.end(), m_registry};
            }

            auto rbegin() noexcept -> iterator
            {
                auto& pool = m_viewBasis->EntityPool();
                return iterator{pool.rbegin(), pool.rend(), m_registry};
            }

            auto rend() noexcept -> iterator
            {
                auto& pool = m_viewBasis->EntityPool();
                return iterator{pool.rend(), pool.rend(), m_registry};
            }

        private:
            Registry* m_registry;
            storage* m_viewBasis;

            auto find_basis_storage() -> storage*
            {
                return std::min<storage*>({m_registry->StorageFor<Ts>()...}, [](const auto* l, const auto* r) { return l->size() < r->size(); });
            }
    };

    /**
     * @brief A view over a single component.
     * 
     * Represents a range of all components of the specified type. Iterator
     * invalidation rules are the same as the registry.
     */
    template<PooledComponent T>
    class view<T>
    {
        public:
            using iterator = detail::single_view_iterator<T>;

            view(Registry* registry)
                : m_registry{registry}
            {
            }

            auto begin() noexcept -> iterator
            {
                auto& pool = m_registry->StorageFor<T>()->ComponentPool();
                return iterator{pool.begin()};
            }

            auto end() noexcept -> iterator
            {
                auto& pool = m_registry->StorageFor<T>()->ComponentPool();
                return iterator{pool.end()};
            }

        private:
            Registry* m_registry;
    };
}