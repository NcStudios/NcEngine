#pragma once

// Implementation of c++23 std::enumerate based on: https://github.com/cor3ntin/rangesnext

#include <ranges>

namespace nc::algo::detail {
template<class... V>
consteval auto iter_cat()
{
    if constexpr ((std::ranges::random_access_range<V> && ...))
    {
        return std::random_access_iterator_tag{};
    }
    else if constexpr ((std::ranges::bidirectional_range<V> && ...))
    {
        return std::bidirectional_iterator_tag{};
    }
    else if constexpr ((std::ranges::forward_range<V> && ...))
    {
        return std::forward_iterator_tag{};
    }
    else if constexpr ((std::ranges::input_range<V> && ...))
    {
        return std::input_iterator_tag{};
    }
    else
    {
        return std::output_iterator_tag{};
    }
}

template<class R>
concept simple_view =
    std::ranges::view<R> &&
    std::ranges::range<const R> &&
    std::same_as<std::ranges::iterator_t<R>, std::ranges::iterator_t<const R>> &&
    std::same_as<std::ranges::sentinel_t<R>, std::ranges::sentinel_t<const R>>;

template<std::ranges::input_range V>
    requires std::ranges::view<V>
class enumerate_view : public std::ranges::view_interface<enumerate_view<V>>
{

    V m_base = {};

    template<bool>
    struct sentinel;

    template<bool Const>
    struct iterator
    {
      private:
        using Base = std::conditional_t<Const, const V, V>;
        using count_type = decltype([] {
            if constexpr (std::ranges::sized_range<Base>)
                return std::ranges::range_size_t<Base>();
            else {
                return std::make_unsigned_t<std::ranges::range_difference_t<Base>>();
            }
        }());

        template<typename T>
        struct result
        {
            const count_type index;
            T value;

            constexpr bool operator==(const result& other) const = default;
        };

        std::ranges::iterator_t<Base> m_it = std::ranges::iterator_t<Base>();
        count_type m_pos = 0;

        template <bool>
        friend struct iterator;
        template <bool>
        friend struct sentinel;

      public:
        using iterator_category = decltype(iter_cat<Base>());
        using reference = result<std::ranges::range_reference_t<Base>>;
        using value_type = result<std::ranges::range_reference_t<Base>>;
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr explicit iterator(std::ranges::iterator_t<Base> current, std::ranges::range_difference_t<Base> pos)
            : m_it(std::move(current)),
              m_pos(static_cast<count_type>(pos))
        {
        }

        constexpr iterator(iterator<!Const> i)
            requires Const &&
                     std::convertible_to<std::ranges::iterator_t<V>, std::ranges::iterator_t<Base>>
            : m_it(std::move(i.m_it)),
              m_pos(i.m_pos)
        {
        }

        constexpr auto base() const & -> const std::ranges::iterator_t<V>&
            requires std::copyable<std::ranges::iterator_t<Base>>
        {
            return m_it;
        }

        constexpr auto base() && -> std::ranges::iterator_t<V>
        {
            return std::move(m_it);
        }

        constexpr auto operator*() const
        {
            return reference{static_cast<count_type>(m_pos), *m_it};
        }

        constexpr auto operator++() -> iterator&
        {
            ++m_pos;
            ++m_it;
            return *this;
        }

        constexpr auto operator++(int)
        {
            ++m_pos;
            if constexpr (std::ranges::forward_range<V>)
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }
            else
            {
                ++m_it;
            }
        }

        constexpr auto operator--() -> iterator&
            requires std::ranges::bidirectional_range<V>
        {
            --m_pos;
            --m_it;
            return *this;
        }

        constexpr auto operator--(int)
            requires std::ranges::bidirectional_range<V>
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        constexpr auto operator+=(difference_type n) -> iterator&
            requires std::ranges::random_access_range<V>
        {
            m_it += n;
            m_pos += n;
            return *this;
        }

        constexpr auto operator-=(difference_type n) -> iterator&
            requires std::ranges::random_access_range<V>
        {
            m_it -= n;
            m_pos -= n;
            return *this;
        }

        friend constexpr auto operator+(const iterator& i, difference_type n) -> iterator
            requires std::ranges::random_access_range<V>
        {
            return iterator{i.m_it + n, static_cast<difference_type>(i.m_pos + n)};
        }

        friend constexpr auto operator+(difference_type n, const iterator &i) -> iterator
            requires std::ranges::random_access_range<V>
        {
            return iterator{i.m_it + n, static_cast<difference_type>(i.m_pos + n)};
        }

        friend constexpr auto operator-(iterator i, difference_type n)
            requires std::ranges::random_access_range<V>
        {
            return iterator{i.m_it - n, static_cast<difference_type>(i.m_pos - n)};
        }

        friend constexpr auto operator-(difference_type n, iterator i)
            requires std::ranges::random_access_range<V>
        {
            return iterator{i.m_it - n, static_cast<difference_type>(i.m_pos - n)};
        }

        constexpr decltype(auto) operator[](difference_type n) const
            requires std::ranges::random_access_range<Base>
        {
            return reference{static_cast<count_type>(m_pos + n), *(m_it + n)};
        }

        friend constexpr auto operator==(const iterator& x, const iterator& y) -> bool
            requires std::equality_comparable<std::ranges::iterator_t<Base>>
        {
            return x.m_it == y.m_it;
        }

        template <bool ConstS>
        friend constexpr auto operator==(const iterator<Const>& i, const sentinel<ConstS>& s) -> bool
        {
            return i.m_it == s.base();
        }

        friend constexpr auto operator<(const iterator& x, const iterator& y) -> bool
            requires std::ranges::random_access_range<Base>
        {
            return x.m_it < y.m_it;
        }

        friend constexpr auto operator>(const iterator& x, const iterator& y) -> bool
            requires std::ranges::random_access_range<Base>
        {
            return x.m_it > y.m_it;
        }

        friend constexpr auto operator<=(const iterator& x, const iterator& y) -> bool
            requires std::ranges::random_access_range<Base>
        {
            return x.m_it <= y.m_it;
        }

        friend constexpr auto operator>=(const iterator& x, const iterator& y) -> bool
            requires std::ranges::random_access_range<Base>
        {
            return x.m_it >= y.m_it;
        }

        friend constexpr auto operator<=>(const iterator& x, const iterator& y)
            requires std::ranges::random_access_range<Base> &&
                     std::three_way_comparable<std::ranges::iterator_t<Base>>
        {
            return x.m_it <=> y.m_it;
        }

        friend constexpr auto operator-(const iterator& x, const iterator& y) -> difference_type
            requires std::ranges::random_access_range<Base>
        {
            return x.m_it - y.m_it;
        }
    };

    template<bool Const>
    struct sentinel {
      private:
        friend iterator<false>;
        friend iterator<true>;

        using Base = std::conditional_t<Const, const V, V>;

        std::ranges::sentinel_t<V> end_;

      public:
        sentinel() = default;

        constexpr explicit sentinel(std::ranges::sentinel_t<V> end)
            : end_(std::move(end))
        {
        }

        constexpr auto base() const
        {
            return end_;
        }

        friend constexpr auto operator-(const iterator<Const> &x, const sentinel &y) -> std::ranges::range_difference_t<Base>
            requires std::sized_sentinel_for<std::ranges::sentinel_t<Base>, std::ranges::iterator_t<Base>> {
            return x.m_it - y.end_;
        }

        friend constexpr auto operator-(const sentinel &x, const iterator<Const> &y) -> std::ranges::range_difference_t<Base>
            requires std::sized_sentinel_for<std::ranges::sentinel_t<Base>, std::ranges::iterator_t<Base>>
        {
            return x.end_ - y.m_it;
        }
    };

  public:
    constexpr enumerate_view() = default;
    constexpr enumerate_view(V base)
        : m_base(std::move(base))
    {
    }

    constexpr auto begin() requires(!simple_view<V>)
    {
        return iterator<false>(std::ranges::begin(m_base), 0);
    }

    constexpr auto begin() const requires simple_view<V>
    {
        return iterator<true>(std::ranges::begin(m_base), 0);
    }

    constexpr auto end()
    {
        return sentinel<false>{std::ranges::end(m_base)};
    }

    constexpr auto end()
        requires std::ranges::common_range<V>
    {
        return iterator<false>{std::ranges::end(m_base), static_cast<std::ranges::range_difference_t<V>>(size())};
    }

    constexpr auto end() const
        requires std::ranges::range<const V>
    {
        return sentinel<true>{std::ranges::end(m_base)};
    }

    constexpr auto end() const
        requires std::ranges::common_range<const V>
    {
        return iterator<true>{std::ranges::end(m_base), static_cast<std::ranges::range_difference_t<V>>(size())};
    }

    constexpr auto size() requires std::ranges::sized_range<V>
    {
        return std::ranges::size(m_base);
    }

    constexpr auto size() const
        requires std::ranges::sized_range<const V>
    {
        return std::ranges::size(m_base);
    }

    constexpr auto base() const & -> V
        requires std::copyable<V>
    {
        return m_base;
    }

    constexpr V base() &&
    {
        return std::move(m_base);
    }
};

template<typename R>
requires std::ranges::input_range<R> enumerate_view(R &&r)
    -> enumerate_view<std::ranges::views::all_t<R>>;

struct enumerate_view_fn {
    template<typename R>
    constexpr auto operator()(R &&r) const {
        return enumerate_view{std::forward<R>(r)};
    }

    template<std::ranges::input_range R>
    constexpr friend auto operator|(R &&rng, const enumerate_view_fn &) {
        return enumerate_view{std::forward<R>(rng)};
    }
};
} // namespace nc::algo::detail
