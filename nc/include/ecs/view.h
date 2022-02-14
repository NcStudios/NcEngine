#pragma once

#include "detail/view_iterator.h"

namespace nc
{
    /**
     * @brief A view over a single component.
     * 
     * Represents a range of all components of the specified type. Iterator
     * invalidation rules are the same as the registry.
     * 
     * @tparam T The component, which models PooledComponent, to be viewed.
     */
    template<PooledComponent T>
    class view
    {
        public:
            using iterator = detail::single_view_iterator<T>;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using storage = detail::PerComponentStorage<T>;

            /**
             * @brief Constructs a new view.
             * @param registry Pointer to the Registry the view will be constructed from.
             */
            view(Registry* registry)
                : m_viewBasis{registry->StorageFor<T>()}
            {
            }

            /**
             * @brief Returns an iterator to the first component in the view.
             * @return iterator
             */
            auto begin() noexcept
            {
                auto& pool = m_viewBasis->ComponentPool();
                return iterator{pool.data()};
            }

            /**
             * @brief Returns an iterator one past the last component in the view.
             * @return iterator
             */
            auto end() noexcept
            {
                auto& pool = m_viewBasis->ComponentPool();
                return iterator{pool.data() + pool.size()};
            }

            /**
             * @brief Returns a reverse iterator to the first component in the reversed view.
             * @return reverse_iterator
             */
            auto rbegin() noexcept
            {
                return std::make_reverse_iterator(end());
            }

            /**
             * @brief Returns a reverse iterator one before the last component in the reversed view.
             * @return reverse_iterator
             */
            auto rend() noexcept
            {
                return std::make_reverse_iterator(begin());
            }

            /**
             * @brief Returns the number of components in the view.
             * @return size_t
             */
            auto size() const noexcept
            {
                return m_viewBasis->size();
            }

        private:
            storage* m_viewBasis;
    };

    /**
     * @brief A view over multiple components.
     * 
     * Represents a range of all entities containing each of the specified components. Iteration
     * invalidation rules are the same as the registry.
     * 
     * @tparam Ts The components, which model PooledComponent, to be viewed.
     */
    template<PooledComponent... Ts>
    class multi_view
    {
        public:
            using iterator = detail::multi_view_iterator<Ts...>;
            using storage = detail::PerComponentStorageBase;

            /**
             * @brief Constructs a new multi_view.
             * @param registry Pointer to the Registry the view will be constructed from.
             */
            multi_view(Registry* registry)
                : m_registry{registry},
                  m_viewBasis{find_basis_storage()}
            {
            }

            /**
             * @brief Returns an iterator to the first entity in the view.
             * @return iterator
             */
            auto begin() noexcept -> iterator
            {
                auto& pool = m_viewBasis->EntityPool();
                return iterator{pool.begin(), pool.end(), m_registry};
            }

            /**
             * @brief Returns an iterator one past the last entity in the view.
             * @return iterator
             */
            auto end() noexcept -> iterator
            {
                auto& pool = m_viewBasis->EntityPool();
                return iterator{pool.end(), pool.end(), m_registry};
            }

            /**
             * @brief Returns the maximum number of entities in the view.
             * @return size_t
             */
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