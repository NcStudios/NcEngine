#pragma once

#include "ncengine/ecs/Registry.h"

#include <iterator>
#include <tuple>

namespace nc::ecs::detail
{
/** Map constness of Reference onto Target. */
template<class Target, class Reference>
struct constness_of
{
    using type = std::conditional_t<std::is_const_v<Reference>,
                                    std::add_const_t<Target>,
                                    std::remove_const_t<Target>>;
};

/** Iterator for a single storage pool. */
template<Viewable T>
class SingleViewIterator final
{
    public:
        using iterator_type = T*;
        using difference_type = std::iterator_traits<iterator_type>::difference_type;
        using value_type = std::iterator_traits<iterator_type>::value_type;
        using pointer = std::iterator_traits<iterator_type>::pointer;
        using reference = std::iterator_traits<iterator_type>::reference;
        using iterator_category = std::contiguous_iterator_tag;

        SingleViewIterator(iterator_type it) noexcept
            : m_cur{it}
        {
        }

        auto operator++() noexcept -> SingleViewIterator&
        {
            ++m_cur;
            return *this;
        }

        auto operator--() noexcept -> SingleViewIterator&
        {
            --m_cur;
            return *this;
        }

        auto operator++(int) noexcept -> SingleViewIterator
        {
            auto temp = *this;
            operator++();
            return temp;
        }

        auto operator--(int) noexcept -> SingleViewIterator
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

        [[nodiscard]] friend auto operator-(const SingleViewIterator<T>& lhs, const SingleViewIterator<T>& rhs) -> difference_type
        {
            return difference_type{lhs.m_cur - rhs.m_cur};
        }

        [[nodiscard]] friend bool operator==(const SingleViewIterator<T>& lhs, const SingleViewIterator<T>& rhs)
        {
            return lhs.m_cur == rhs.m_cur;
        }

        [[nodiscard]] friend bool operator!=(const SingleViewIterator<T>& lhs, const SingleViewIterator<T>& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        iterator_type m_cur;
};

template<PooledComponent... Ts>
class MultiViewIterator final
{
    public:
        using basis_iterator = ComponentPoolBase::entity_iterator;
        using value_type = std::tuple<Ts*...>;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        MultiViewIterator(basis_iterator beg, basis_iterator end, Registry* registry) noexcept
            : m_cur{beg}, m_end{end}, m_registry{registry}, m_currentValues{}
        {
            if(m_cur != m_end && !fill_values()) operator++();
        }

        auto operator++() noexcept -> MultiViewIterator&
        {
            while(++m_cur != m_end && !fill_values()) {}
            return *this;
        }

        auto operator++(int) noexcept -> MultiViewIterator
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

        [[nodiscard]] friend bool operator==(const MultiViewIterator<Ts...>& lhs, const MultiViewIterator<Ts...>& rhs)
        {
            return lhs.m_cur == rhs.m_cur;
        }

        [[nodiscard]] friend bool operator!=(const MultiViewIterator<Ts...>& lhs, const MultiViewIterator<Ts...>& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        basis_iterator m_cur;
        basis_iterator m_end;
        Registry* m_registry;
        value_type m_currentValues;

        template<class T>
        using raw_type = std::remove_const_t<std::remove_pointer_t<std::remove_reference_t<T>>>;

        bool fill_values()
        {
            auto entity = *m_cur;
            auto registry = m_registry;
            return std::apply([entity, registry](auto*&... element)
            {
                auto trySetComponent = [](auto*& dst, Entity ent, Registry* reg)
                {
                    using component_t = raw_type<decltype(dst)>;
                    dst = reg->Get<component_t>(ent);
                    return dst != nullptr;
                };

                return (trySetComponent(element, entity, registry) && ...);
            }, m_currentValues);
        }
};

/** Generalized access to registry storage for views over multiple components. */
template<Viewable... Ts>
struct ViewStorageAdaptor
{
    using iterator = detail::MultiViewIterator<Ts...>;
    using storage_type = ComponentPoolBase;

    static auto basis(Registry* registry) -> storage_type*
    {
        return std::min<storage_type*>
        (
            {registry->StorageFor<std::remove_const_t<Ts>>()...},
            [](const auto* l, const auto* r) { return l->Size() < r->Size(); }
        );
    }

    static auto begin(storage_type* basis, Registry* registry) noexcept -> iterator
    {
        auto entities = basis->GetEntityPool();
        return iterator{std::begin(entities), std::end(entities), registry};
    }

    static auto end(storage_type* basis, Registry* registry) noexcept -> iterator
    {
        auto entities = basis->GetEntityPool();
        return iterator{std::end(entities), std::end(entities), registry};
    }
};

/** Specialization for views over entities. */
template<std::same_as<Entity> T>
struct ViewStorageAdaptor<T>
{
    using value_type = std::remove_const_t<T>;
    using iterator = detail::SingleViewIterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using storage_type = constness_of<EntityPool, T>::type;

    static auto basis(Registry* registry) -> storage_type*
    {
        return registry->StorageFor<value_type>();
    }

    static auto basis(const Registry* registry) -> const storage_type*
    {
        return registry->StorageFor<value_type>();
    }

    static auto begin(storage_type* basis) noexcept -> iterator
    {
        return iterator{basis->data()};
    }

    static auto end(storage_type* basis) noexcept -> iterator
    {
        return iterator{basis->data() + basis->size()};
    }
};

/** Specialization for views over single components. */
template<PooledComponent T>
struct ViewStorageAdaptor<T>
{
    using value_type = std::remove_const_t<T>;
    using iterator = detail::SingleViewIterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using storage_type = constness_of<ComponentPool<std::remove_const_t<T>>, T>::type;

    static auto basis(Registry* registry) -> storage_type*
    {
        return registry->StorageFor<value_type>();
    }

    static auto basis(const Registry* registry) -> storage_type*
    {
        return registry->StorageFor<value_type>();
    }

    static auto begin(storage_type* basis) noexcept -> iterator
    {
        auto pool = basis->GetComponents();
        return iterator{pool.data()};
    }

    static auto end(storage_type* basis) noexcept -> iterator
    {
        auto pool = basis->GetComponents();
        return iterator{pool.data() + pool.size()};
    }
};
} // namespace nc::ecs::detail
