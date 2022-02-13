#pragma once

#include "detail/view_iterator.h"

namespace nc
{
    /**
     * @brief A view over a single component.
     * 
     * Represents a range of all components of the specified type. Iterator
     * invalidation rules are the same as the registry.
     */
    template<PooledComponent T>
    class view
    {
        public:
            using iterator = detail::single_view_iterator<T>;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using storage = detail::PerComponentStorage<T>;

            view(Registry* registry)
                : m_viewBasis{registry->StorageFor<T>()}
            {
            }

            auto begin() noexcept { return iterator{m_viewBasis->ComponentPool().begin()}; }
            auto end() noexcept { return iterator{m_viewBasis->ComponentPool().end()}; }
            auto rbegin() noexcept { return reverse_iterator{m_viewBasis->ComponentPool().rbegin()}; }
            auto rend() noexcept { return reverse_iterator{m_viewBasis->ComponentPool().rend()}; }
            auto size() const noexcept { return m_viewBasis->size(); }

        private:
            storage* m_viewBasis;
    };

    /**
     * @brief A view over multiple components.
     * 
     * Represents a range of all entities containing each of the specified components. Iteration
     * invalidation rules are the same as the registry.
     */
    template<PooledComponent... Ts>
    class multi_view
    {
        public:
            using iterator = detail::multi_view_iterator<Ts...>;
            using storage = detail::PerComponentStorageBase;

            multi_view(Registry* registry)
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

            auto size_upper_bound() const noexcept -> size_t
            {
                return m_viewBasis->size();
            }

        private:
            Registry* m_registry;
            storage* m_viewBasis;

            auto find_basis_storage() -> storage*
            {
                return std::min<storage*>({m_registry->StorageFor<Ts>()...}, [](const auto* l, const auto* r) { return l->size() < r->size(); });
            }
    };
}