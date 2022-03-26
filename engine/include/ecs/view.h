#pragma once

#include "detail/view_utility.h"

namespace nc
{
    /**
     * @brief A view over a single registry type.
     * 
     * Represents a contiguous range of all objects of the specified type. Iterator
     * invalidation rules are the same as the registry.
     * 
     * @tparam T The type, which models viewable, to be accessed.
     */
    template<viewable T>
    class view
    {
        public:
            using adaptor = detail::view_storage_adaptor<T>;
            using storage = adaptor::storage_type;

            /**
             * @brief Constructs a new view.
             * @param registry Pointer to a Registry the view will be constructed from.
             */
            view(Registry* registry)
                : m_viewBasis{adaptor::basis(registry)} {}

            /**
             * @brief Constructs a new read-only view.
             * @param registry Pointer to a const Registry the view will be constructed from.
             */
            view(const Registry* registry)
                : m_viewBasis{adaptor::basis(registry)} {}

            /**
             * @brief Returns an iterator to the first element in the view.
             * @return iterator
             */
            auto begin() const noexcept { return adaptor::begin(m_viewBasis); }

            /**
             * @brief Returns an iterator one past the last element in the view.
             * @return iterator
             */
            auto end() const noexcept { return adaptor::end(m_viewBasis); }

            /**
             * @brief Returns a reverse iterator to the first element in the reversed view.
             * @return reverse_iterator
             */
            auto rbegin() const noexcept { return std::make_reverse_iterator(end()); }

            /**
             * @brief Returns a reverse iterator one before the last element in the reversed view.
             * @return reverse_iterator
             */
            auto rend() const noexcept { return std::make_reverse_iterator(begin()); }

            /**
             * @brief Returns the number of elements in the view.
             * @return size_t
             */
            auto size() const noexcept { return m_viewBasis->size(); }

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
            using adaptor = detail::view_storage_adaptor<Ts...>;
            using storage = adaptor::storage_type;

            /**
             * @brief Constructs a new multi_view.
             * @param registry Pointer to the Registry the view will be constructed from.
             */
            multi_view(Registry* registry)
                : m_registry{registry}, m_viewBasis{adaptor::basis(registry)} {}

            /**
             * @brief Returns an iterator to the first entity in the view.
             * @return iterator
             */
            auto begin() noexcept { return adaptor::begin(m_viewBasis, m_registry); }

            /**
             * @brief Returns an iterator one past the last entity in the view.
             * @return iterator
             */
            auto end() noexcept { return adaptor::end(m_viewBasis, m_registry); }

            /**
             * @brief Returns the maximum number of entities in the view.
             * @return size_t
             */
            auto size_upper_bound() const noexcept { return m_viewBasis->size(); }

        private:
            Registry* m_registry;
            storage* m_viewBasis;
    };
}