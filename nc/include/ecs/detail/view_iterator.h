#pragma once

#include "ecs/Registry.h"

#include <iterator>

namespace nc::detail
{
    template<class Target, class Reference>
    struct constness_as_other
    {
        using type = std::conditional_t<std::is_const_v<Reference>, const Target, Target>;
    };

    template<PooledComponent T>
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
            using iterator_type = entity_storage::iterator;
            using difference_type = std::iterator_traits<iterator_type>::difference_type;
            using value_type = std::iterator_traits<iterator_type>::value_type;
            using pointer = std::iterator_traits<iterator_type>::pointer;
            using reference = std::iterator_traits<iterator_type>::reference;
            using iterator_category = std::forward_iterator_tag;

            multi_view_iterator(iterator_type beg, iterator_type end, Registry* registry) noexcept
                : m_cur{beg}, m_end{end}, m_registry{registry}
            {
                if(m_cur != m_end && !valid()) operator++();
            }

            auto operator++() noexcept -> multi_view_iterator&
            {
                while(++m_cur != m_end && !valid()) {}
                return *this;
            }

            auto operator++(int) noexcept -> multi_view_iterator
            {
                auto temp = *this;
                operator++();
                return temp;
            }

            [[nodiscard]] auto operator->() const -> pointer
            {
                return &*m_cur;
            }

            [[nodiscard]] auto operator*() const -> reference
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
            iterator_type m_cur;
            iterator_type m_end;
            Registry* m_registry;

            bool valid()
            {
                return (m_registry->Contains<Ts>(*m_cur) && ...);
            }
    };
}