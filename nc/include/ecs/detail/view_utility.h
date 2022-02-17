#pragma once

#include "ecs/Registry.h"

#include <iterator>
#include <tuple>

namespace nc::detail
{
    /** Map constness of Reference onto Target. */
    template<class Target, class Reference>
    struct constness_of
    {
        using type = std::conditional_t<std::is_const_v<Reference>,
                                        std::add_const_t<Target>,
                                        std::remove_const_t<Target>>;
    };

    /** Generalized access to registry storage for views. */
    template<viewable T>
    struct view_storage_adaptor {};

    /** Specialization for views over entities. */
    template<std::same_as<Entity> T>
    struct view_storage_adaptor<T>
    {
        using storage_type = constness_of<detail::entity_storage, T>::type;

        static auto begin(storage_type* basis) noexcept
        {
            auto& pool = basis->pool();
            return pool.data();
        }

        static auto end(storage_type* basis) noexcept
        {
            auto& pool = basis->pool();
            return pool.data() + pool.size();
        }
    };

    /** Specialization for views over components. */
    template<PooledComponent T>
    struct view_storage_adaptor<T>
    {
        using storage_type = constness_of<detail::PerComponentStorage<std::remove_const_t<T>>, T>::type;

        static auto begin(storage_type* basis) noexcept
        {
            auto& pool = basis->ComponentPool();
            return pool.data();
        }

        static auto end(storage_type* basis) noexcept
        {
            auto& pool = basis->ComponentPool();
            return pool.data() + pool.size();
        }
    };

    /** Iterator for a single storage pool. */
    template<viewable T>
    class single_view_iterator final
    {
        public:
            using iterator_type = T*;
            using difference_type = std::iterator_traits<iterator_type>::difference_type;
            using value_type = std::iterator_traits<iterator_type>::value_type;
            using pointer = std::iterator_traits<iterator_type>::pointer;
            using reference = std::iterator_traits<iterator_type>::reference;
            using iterator_category = std::contiguous_iterator_tag;

            single_view_iterator(iterator_type it) noexcept
                : m_cur{it}
            {
            }

            auto operator++() noexcept -> single_view_iterator&
            {
                ++m_cur;
                return *this;
            }

            auto operator--() noexcept -> single_view_iterator&
            {
                --m_cur;
                return *this;
            }

            auto operator++(int) noexcept -> single_view_iterator
            {
                auto temp = *this;
                operator++();
                return temp;
            }

            auto operator--(int) noexcept -> single_view_iterator
            {
                auto temp = *this;
                operator--();
                return temp;
            }

            [[nodiscard]] auto operator->() const -> pointer
            {
                return m_cur;
            }

            [[nodiscard]] auto operator*() const -> reference
            {
                return *m_cur;
            }

            [[nodiscard]] friend bool operator==(const single_view_iterator<T>& lhs, const single_view_iterator<T>& rhs)
            {
                return lhs.m_cur == rhs.m_cur;
            }

            [[nodiscard]] friend bool operator!=(const single_view_iterator<T>& lhs, const single_view_iterator<T>& rhs)
            {
                return !(lhs == rhs);
            }

        private:
            iterator_type m_cur;
    };

    template<PooledComponent... Ts>
    class multi_view_iterator final
    {
        public:
            using basis_iterator = entity_storage::iterator;
            using value_type = std::tuple<Ts*...>;
            using pointer = value_type*;
            using const_pointer = const value_type*;
            using reference = value_type&;
            using const_reference = const value_type&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            multi_view_iterator(basis_iterator beg, basis_iterator end, Registry* registry) noexcept
                : m_cur{beg}, m_end{end}, m_registry{registry}, m_currentValues{}
            {
                if(m_cur != m_end && !fill_values()) operator++();
            }

            auto operator++() noexcept -> multi_view_iterator&
            {
                while(++m_cur != m_end && !fill_values()) {}
                return *this;
            }

            auto operator++(int) noexcept -> multi_view_iterator
            {
                auto temp = *this;
                operator++();
                return temp;
            }

            [[nodiscard]] auto operator->() -> pointer
            {
                return &m_currentValues;
            }

            [[nodiscard]] auto operator->() const -> const_pointer
            {
                return &m_currentValues;
            }

            [[nodiscard]] auto operator*() -> reference
            {
                return *operator->();
            }

            [[nodiscard]] auto operator*() const -> const_reference
            {
                return *operator->();
            }

            [[nodiscard]] friend bool operator==(const multi_view_iterator<Ts...>& lhs, const multi_view_iterator<Ts...>& rhs)
            {
                return lhs.m_cur == rhs.m_cur;
            }

            [[nodiscard]] friend bool operator!=(const multi_view_iterator<Ts...>& lhs, const multi_view_iterator<Ts...>& rhs)
            {
                return !(lhs == rhs);
            }

        private:
            basis_iterator m_cur;
            basis_iterator m_end;
            Registry* m_registry;
            value_type m_currentValues;

            bool fill_values()
            {
                return std::apply([ent = *m_cur, reg = m_registry](auto*&... element)
                {
                    return ((element = reg->Get<std::remove_pointer_t<std::remove_reference_t<decltype(element)>>>(ent)) && ...);
                }, m_currentValues);
            }
    };
}