/**
 * @file View.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Ecs.h"
#include "detail/ViewUtility.h"


#include <ranges>

namespace nc::ecs
{

// want at least zip + sparse_zip (names? inner_join + outer_join, stick with GetAll?)
//   maybe GetJoined() and GetJoinedSparse()
// if basis is begin/end iterator, we can also construct from filtered sets
//   in this case, we could take an optional filter to apply before collecting components or could
//   just expect user to do filtering
// maybe have ctor take pools in, and expose func through Ecs
// can we propagate const (if component is const, we get const pool and return const*) ?
// want to constraint on > 0 types; msvc does: requires (view<_ViewTypes> && ...) && (sizeof...(_ViewTypes) > 0)

inline auto PoolLessThan(ComponentPoolBase* lhs, ComponentPoolBase* rhs) -> bool
{
    return lhs->Size() < rhs->Size();
}

/** @brief Utility to get the smallest Entity pool from a set of Component types. */
template<PooledComponent... Ts, class EcsType>
auto GetSmallestEntityPool(EcsType& world) -> std::span<const Entity>
{
    return std::min<ecs::ComponentPoolBase*>(
        {&world.template GetPool<std::remove_const_t<Ts>>()...},
        PoolLessThan
    )->GetEntityPool();
}

/** @brief Utility to get the largest Entity pool from a set of Component types. */
template<PooledComponent... Ts, class EcsType>
auto GetLargestEntityPool(EcsType& world) -> std::span<const Entity>
{
    return std::max<ecs::ComponentPoolBase*>(
        {&world.template GetPool<std::remove_const_t<Ts>>()...},
        PoolLessThan
    )->GetEntityPool();
}

/**
 * @brief Some information yet to do
 * 
 * @par Additional functions inherited from `std::ranges::view_interface` (both const and mutable versions):
 * - `[[nodiscard]] auto cbegin()`
 * - `[[nodiscard]] auto cend()`
 * - `[[nodiscard]] auto front()`
 * - `[[nodiscard]] auto empty() -> bool`
 * - `[[nodiscard]] explicit operator bool()`
 */
template<class EcsType, PooledComponent... Ts>
    requires (sizeof...(Ts) > 0)
class InnerJoinView : public std::ranges::view_interface<InnerJoinView<EcsType, Ts...>>
{
    public:
        /** @brief Iterator value type. */
        using value_type = std::tuple<Ts*...>;

        /** @brief Construct a joined view of the components given an EcsInterface instance. */
        explicit InnerJoinView(EcsType ecs)
            : m_ecs{ecs},
              m_basis{GetSmallestEntityPool<Ts...>(ecs)}
        {
        }

        /** @brief Get an iterator to the beginning of the range. */
        auto begin() const noexcept
        {
            return Iterator{std::begin(m_basis), std::end(m_basis), &m_ecs};
        }

        /** @brief Get an iterator to the end of the range. */
        auto end() const noexcept
        {
            return Sentinel{std::end(m_basis)};
        }

        /** @brief Get the maximum number of elements that may be in the range. */
        auto size_upper_bound() const noexcept { return m_basis.size(); }

        /**
         * @brief Utility to fill a tuple of component pointers for a given Entity.
         * @return True if the operation was completely successfuly (the Entity had all types).
         */
        static auto join_for(EcsType ecs, Entity target, value_type& value) -> bool
        {
            auto collect = [&ecs, target]<class P>(P& dst)
            {
                using T = std::remove_pointer_t<P>;
                if (ecs.Contains<T>(target))
                {
                    dst = &ecs.Get<T>(target);
                    return true;
                }

                return false;
            };

            return std::apply(
                [&collect](auto&... dst) {
                    return (collect(dst) && ...);
                },
                value
            );
        }

    private:
        using basis_type = std::span<const Entity>;
        using basis_iterator = typename basis_type::iterator;

        mutable EcsType m_ecs;
        std::span<const Entity> m_basis;

        struct Sentinel
        {
            Sentinel() = default;

            explicit Sentinel(basis_iterator end_)
                : end{end_} {}

            basis_iterator end;
        };

        class Iterator
        {
            public:
                using iterator_category = std::forward_iterator_tag;
                using value_type = typename InnerJoinView<EcsType, Ts...>::value_type;
                using pointer = value_type*;
                using const_pointer = const value_type*;
                using difference_type = std::ptrdiff_t;

                Iterator() = default;

                Iterator(basis_iterator beg, basis_iterator end, EcsType* ecs) noexcept
                    : m_cur{beg}, m_end{end}, m_ecs{ecs}, m_set{}
                {
                    if (m_cur != m_end && !fill_values())
                        operator++();
                }

                auto operator++() noexcept -> Iterator&
                {
                    while(++m_cur != m_end && !fill_values()) {}
                    return *this;
                }

                auto operator++(int) noexcept -> Iterator
                {
                    auto temp = *this;
                    operator++();
                    return temp;
                }

                [[nodiscard]] auto operator->()       noexcept -> pointer       { return &m_set;  }
                [[nodiscard]] auto operator->() const noexcept -> const_pointer { return &m_set;  }
                [[nodiscard]] auto operator*()        noexcept -> value_type    { return *operator->(); }
                [[nodiscard]] auto operator*()  const noexcept -> value_type    { return *operator->(); }
                [[nodiscard]] friend bool operator==(const Iterator& l, const Iterator& r) noexcept { return l.m_cur == r.m_cur; }
                [[nodiscard]] friend bool operator!=(const Iterator& l, const Iterator& r) noexcept { return l.m_cur != r.m_cur; }
                [[nodiscard]] friend bool operator==(const Iterator& i, const Sentinel& s) noexcept { return i.m_cur == s.end;   }
                [[nodiscard]] friend bool operator!=(const Iterator& i, const Sentinel& s) noexcept { return i.m_cur != s.end;   }
                [[nodiscard]] friend bool operator==(const Sentinel& s, const Iterator& i) noexcept { return i.m_cur == s.end;   }
                [[nodiscard]] friend bool operator!=(const Sentinel& s, const Iterator& i) noexcept { return i.m_cur != s.end;   }

            private:
                basis_iterator m_cur{};
                basis_iterator m_end{};
                EcsType* m_ecs = nullptr;
                value_type m_set{};

                auto fill_values() -> bool
                {
                    return join_for(*m_ecs, *m_cur, m_set);
                }
        };
};

/**
 * @brief Get a view of tuples 
 * @return 
 */
template<PooledComponent... Ts, class EcsType>
auto InnerJoin(EcsType ecs) -> InnerJoinView<EcsType, Ts...>
{
    return InnerJoinView<EcsType, Ts...>{ecs};
}

/**
 * @brief 
 * @return 
 * 
 */
template<PooledComponent... Ts, class EcsType>
auto InnerJoin(EcsType ecs, Entity entity) -> InnerJoinView<EcsType, Ts...>::value_type
{
    using view = InnerJoinView<EcsType, Ts...>;
    auto set = typename view::value_type{};
    const auto result = view::join_for(ecs, entity, set);
    NC_ASSERT(result, "Entity did not have all specified components.");
    return set;
}
} // nc::ecs

namespace nc
{
/**
 * @brief A view over a single registry type.
 * 
 * Represents a contiguous range of all objects of the specified type. Iterator
 * invalidation rules are the same as the registry.
 * 
 * @tparam T The type, which models Viewable, to be accessed.
 */
template<Viewable T>
class View
{
    public:
        using adaptor = ecs::detail::ViewStorageAdaptor<T>;
        using storage = adaptor::storage_type;

        /**
         * @brief Constructs a new view.
         * @param registry Pointer to a Registry the view will be constructed from.
         */
        View(Registry* registry)
            : m_viewBasis{adaptor::basis(registry)} {}

        /**
         * @brief Constructs a new read-only view.
         * @param registry Pointer to a const Registry the view will be constructed from.
         */
        View(const Registry* registry)
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
        auto size() const noexcept { return m_viewBasis->Size(); }

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
class MultiView
{
    public:
        using adaptor = ecs::detail::ViewStorageAdaptor<Ts...>;
        using storage = adaptor::storage_type;

        /**
         * @brief Constructs a new MultiView.
         * @param registry Pointer to the Registry the view will be constructed from.
         */
        MultiView(Registry* registry)
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
        auto size_upper_bound() const noexcept { return m_viewBasis->Size(); }

    private:
        Registry* m_registry;
        storage* m_viewBasis;
};
} // namespace nc
