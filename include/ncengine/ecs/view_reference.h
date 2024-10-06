   
   
   template <input_range... _ViewTypes>
        requires (view<_ViewTypes> && ...) && (sizeof...(_ViewTypes) > 0)
    class zip_view : public view_interface<zip_view<_ViewTypes...>> {
    private:
        /* [[no_unique_address]] */ tuple<_ViewTypes...> _Views;

        template <bool _IsConst>
        struct _Category_base {};

        template <bool _IsConst>
            requires _All_forward<_IsConst, _ViewTypes...>
        struct _Category_base<_IsConst> {
            using iterator_category = input_iterator_tag;
        };

        template <bool _IsConst>
        class _Iterator : public _Category_base<_IsConst> {
        private:
            friend zip_view;

            template <class _Func, class... _OtherViews>
                requires _Zip_transform_constraints<_Func, _OtherViews...>
            friend class zip_transform_view;

            using _My_tuple = tuple<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>...>;

            /* [[no_unique_address]] */ _My_tuple _Current;

            constexpr explicit _Iterator(_My_tuple _Current_) noexcept(
                is_nothrow_move_constructible_v<_My_tuple>) // strengthened
                : _Current(_STD move(_Current_)) {}

        public:
            using iterator_concept = conditional_t<_All_random_access<_IsConst, _ViewTypes...>,
                random_access_iterator_tag,
                conditional_t<_All_bidirectional<_IsConst, _ViewTypes...>, bidirectional_iterator_tag,
                    conditional_t<_All_forward<_IsConst, _ViewTypes...>, forward_iterator_tag, input_iterator_tag>>>;

            using value_type      = tuple<range_value_t<_Maybe_const<_IsConst, _ViewTypes>>...>;
            using difference_type = common_type_t<range_difference_t<_Maybe_const<_IsConst, _ViewTypes>>...>;

            _Iterator() = default;

            constexpr _Iterator(_Iterator<!_IsConst> _Rhs) noexcept(
                (is_nothrow_convertible_v<iterator_t<_ViewTypes>, iterator_t<const _ViewTypes>> && ...)) // strengthened
                requires (_IsConst && (convertible_to<iterator_t<_ViewTypes>, iterator_t<const _ViewTypes>> && ...))
                : _Current(_STD move(_Rhs._Current)) {}

            _NODISCARD constexpr auto operator*() const
                noexcept((noexcept(*(_STD declval<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>&>()))
                          && ...)) /* strengthened */ {
                return _Tuple_transform(
                    [](auto& _Itr) _STATIC_CALL_OPERATOR noexcept(noexcept(*_Itr)) -> decltype(auto) { return *_Itr; },
                    _Current);
            }

            constexpr _Iterator& operator++() noexcept(
                noexcept(_Tuple_for_each([](auto& _Itr) _STATIC_CALL_OPERATOR noexcept(noexcept(++_Itr)) { ++_Itr; },
                    _Current))) /* strengthened */ {
                _Tuple_for_each([](auto& _Itr) _STATIC_CALL_OPERATOR noexcept(noexcept(++_Itr)) { ++_Itr; }, _Current);
                return *this;
            }

            constexpr void operator++(int) noexcept(noexcept(++(_STD declval<_Iterator>()))) /* strengthened */ {
                ++*this;
            }

            constexpr _Iterator operator++(int) noexcept(
                noexcept(++(_STD declval<_Iterator>())) && is_nothrow_copy_constructible_v<_Iterator>) // strengthened
                requires _All_forward<_IsConst, _ViewTypes...>
            {
                auto _Temp = *this;
                ++*this;

                return _Temp;
            }

            constexpr _Iterator& operator--() noexcept(noexcept(_Tuple_for_each(
                [](auto& _Itr) _STATIC_CALL_OPERATOR noexcept(noexcept(--_Itr)) { --_Itr; }, _Current))) // strengthened
                requires _All_bidirectional<_IsConst, _ViewTypes...>
            {
                _Tuple_for_each([](auto& _Itr) _STATIC_CALL_OPERATOR noexcept(noexcept(--_Itr)) { --_Itr; }, _Current);
                return *this;
            }

            constexpr _Iterator operator--(int) noexcept(
                noexcept(--(_STD declval<_Iterator>())) && is_nothrow_copy_constructible_v<_Iterator>) // strengthened
                requires _All_bidirectional<_IsConst, _ViewTypes...>
            {
                auto _Temp = *this;
                --*this;

                return _Temp;
            }

            constexpr _Iterator& operator+=(const difference_type _Off) noexcept(
                (noexcept(_STD declval<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>&>() +=
                          static_cast<iter_difference_t<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>>>(_Off))
                    && ...)) // strengthened
                requires _All_random_access<_IsConst, _ViewTypes...>
            {
                _Tuple_for_each(
                    [&_Off]<class _IteratorType>(_IteratorType& _Itr) noexcept(
                        noexcept(_Itr += static_cast<iter_difference_t<_IteratorType>>(_Off))) {
                        _Itr += static_cast<iter_difference_t<_IteratorType>>(_Off);
                    },
                    _Current);

                return *this;
            }

            constexpr _Iterator& operator-=(const difference_type _Off) noexcept(
                (noexcept(_STD declval<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>&>() -=
                          static_cast<iter_difference_t<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>>>(_Off))
                    && ...)) // strengthened
                requires _All_random_access<_IsConst, _ViewTypes...>
            {
                _Tuple_for_each(
                    [&_Off]<class _IteratorType>(_IteratorType& _Itr) noexcept(
                        noexcept(_Itr -= static_cast<iter_difference_t<_IteratorType>>(_Off))) {
                        _Itr -= static_cast<iter_difference_t<_IteratorType>>(_Off);
                    },
                    _Current);

                return *this;
            }

            _NODISCARD constexpr auto operator[](const difference_type _Where) const noexcept(
                (noexcept((_STD declval<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>&>())
                         [static_cast<iter_difference_t<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>>>(_Where)])
                    && ...)) // strengthened
                requires _All_random_access<_IsConst, _ViewTypes...>
            {
                return _Tuple_transform(
                    [&_Where]<class _IteratorType>(_IteratorType& _Itr) noexcept(
                        noexcept(_Itr[static_cast<iter_difference_t<_IteratorType>>(_Where)])) -> decltype(auto) {
                        return _Itr[static_cast<iter_difference_t<_IteratorType>>(_Where)];
                    },
                    _Current);
            }

            _NODISCARD_FRIEND constexpr bool operator==(const _Iterator& _Lhs, const _Iterator& _Rhs) noexcept(
                noexcept(_Zip_iterator_sentinel_equal(_Lhs._Current, _Rhs._Current))) // strengthened
                requires (equality_comparable<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>> && ...)
            {
                if constexpr (!_Zip_is_common<_Maybe_const<_IsConst, _ViewTypes>...>
                              || _All_random_access<_IsConst, _ViewTypes...> || sizeof...(_ViewTypes) == 1) {
                    return _STD get<0>(_Lhs._Current) == _STD get<0>(_Rhs._Current);
                } else {
                    return _Zip_iterator_sentinel_equal(_Lhs._Current, _Rhs._Current);
                }
            }

            _NODISCARD_FRIEND constexpr auto operator<=>(const _Iterator& _Lhs, const _Iterator& _Rhs)
                requires _All_random_access<_IsConst, _ViewTypes...>
            {
                return _Lhs._Current <=> _Rhs._Current;
            }

            _NODISCARD_FRIEND constexpr _Iterator operator+(const _Iterator& _Lhs, const difference_type _Rhs) noexcept(
                noexcept(_STD declval<_Iterator&>() += _Rhs)
                && is_nothrow_copy_constructible_v<_Iterator>) // strengthened
                requires _All_random_access<_IsConst, _ViewTypes...>
            {
                auto _Modified_iterator = _Lhs;
                _Modified_iterator += _Rhs;

                return _Modified_iterator;
            }

            _NODISCARD_FRIEND constexpr _Iterator operator+(const difference_type _Lhs, const _Iterator& _Rhs) noexcept(
                noexcept(_Rhs + _Lhs)) /* strengthened */
                requires _All_random_access<_IsConst, _ViewTypes...>
            {
                return _Rhs + _Lhs;
            }

            _NODISCARD_FRIEND constexpr _Iterator operator-(const _Iterator& _Lhs, const difference_type _Rhs) noexcept(
                noexcept(_STD declval<_Iterator&>() -= _Rhs)
                && is_nothrow_copy_constructible_v<_Iterator>) // strengthened
                requires _All_random_access<_IsConst, _ViewTypes...>
            {
                auto _Modified_iterator = _Lhs;
                _Modified_iterator -= _Rhs;

                return _Modified_iterator;
            }

            _NODISCARD_FRIEND constexpr difference_type
                operator-(const _Iterator& _Lhs, const _Iterator& _Rhs) noexcept(
                    noexcept(_Zip_get_smallest_distance<difference_type>(_STD declval<_My_tuple>(),
                        _STD declval<_My_tuple>()))) // strengthened
                requires (sized_sentinel_for<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>,
                              iterator_t<_Maybe_const<_IsConst, _ViewTypes>>>
                          && ...)
            {
                return _Zip_get_smallest_distance<difference_type>(_Lhs._Current, _Rhs._Current);
            }

            _NODISCARD_FRIEND constexpr auto iter_move(const _Iterator& _Itr) noexcept(
                (noexcept(_RANGES iter_move(_STD declval<const iterator_t<_Maybe_const<_IsConst, _ViewTypes>>&>()))
                    && ...)
                && (is_nothrow_move_constructible_v<range_rvalue_reference_t<_Maybe_const<_IsConst, _ViewTypes>>>
                    && ...)) {
                return _Tuple_transform(_RANGES iter_move, _Itr._Current);
            }

            // clang-format off
            friend constexpr void iter_swap(const _Iterator& _Lhs, const _Iterator& _Rhs)
                noexcept((noexcept(_RANGES iter_swap(
                    _STD declval<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>&>(),
                    _STD declval<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>&>())) && ...))
                requires (indirectly_swappable<iterator_t<_Maybe_const<_IsConst, _ViewTypes>>> && ...)
            {
                // clang-format on
                const auto _Swap_every_pair_closure =
                    [&_Lhs, &_Rhs]<size_t... _Indices>(index_sequence<_Indices...>) noexcept(noexcept(
                        ((_RANGES iter_swap(_STD get<_Indices>(_Lhs._Current), _STD get<_Indices>(_Rhs._Current))),
                            ...))) {
                        ((_RANGES iter_swap(_STD get<_Indices>(_Lhs._Current), _STD get<_Indices>(_Rhs._Current))),
                            ...);
                    };

                _Swap_every_pair_closure(index_sequence_for<_ViewTypes...>{});
            }
        };

        template <bool _IsConst>
        class _Sentinel {
        private:
            friend zip_view;

            using _My_tuple = tuple<sentinel_t<_Maybe_const<_IsConst, _ViewTypes>>...>;

            /* [[no_unique_address]] */ _My_tuple _End;

            constexpr explicit _Sentinel(_My_tuple _End_) noexcept(
                is_nothrow_copy_constructible_v<_My_tuple>) // strengthened
                : _End(_End_) {}

            template <bool _IteratorConst>
            _NODISCARD static constexpr const auto& _Get_iterator_tuple(
                const _Iterator<_IteratorConst>& _Itr) noexcept {
                // NOTE: This function is necessary because friend functions are never
                // member functions, and friendship is not transitive.
                return _Itr._Current;
            }

        public:
            _Sentinel() = default;

            constexpr _Sentinel(_Sentinel<!_IsConst> _Rhs) noexcept(
                (is_nothrow_convertible_v<sentinel_t<_ViewTypes>, sentinel_t<const _ViewTypes>> && ...)) // strengthened
                requires (_IsConst && (convertible_to<sentinel_t<_ViewTypes>, sentinel_t<const _ViewTypes>> && ...))
                : _End(_STD move(_Rhs._End)) {}

            template <bool _IteratorConst>
                requires (sentinel_for<sentinel_t<_Maybe_const<_IsConst, _ViewTypes>>,
                              iterator_t<_Maybe_const<_IteratorConst, _ViewTypes>>>
                          && ...)
            _NODISCARD_FRIEND constexpr bool
                operator==(const _Iterator<_IteratorConst>& _Lhs, const _Sentinel& _Rhs) noexcept(
                    noexcept(_Zip_iterator_sentinel_equal(_Get_iterator_tuple(_Lhs), _Rhs._End))) /* strengthened */ {
                return _Zip_iterator_sentinel_equal(_Get_iterator_tuple(_Lhs), _Rhs._End);
            }

            template <bool _IteratorConst>
                requires (sized_sentinel_for<sentinel_t<_Maybe_const<_IsConst, _ViewTypes>>,
                              iterator_t<_Maybe_const<_IteratorConst, _ViewTypes>>>
                          && ...)
            _NODISCARD_FRIEND constexpr common_type_t<range_difference_t<_Maybe_const<_IteratorConst, _ViewTypes>>...>
                operator-(const _Iterator<_IteratorConst>& _Lhs, const _Sentinel& _Rhs) noexcept(
                    noexcept(_Zip_get_smallest_distance<
                        common_type_t<range_difference_t<_Maybe_const<_IteratorConst, _ViewTypes>>...>>(
                        _Get_iterator_tuple(_Lhs), _Rhs._End))) /* strengthened */ {
                using _Difference_type = common_type_t<range_difference_t<_Maybe_const<_IteratorConst, _ViewTypes>>...>;
                return _Zip_get_smallest_distance<_Difference_type>(_Get_iterator_tuple(_Lhs), _Rhs._End);
            }

            template <bool _IteratorConst>
                requires (sized_sentinel_for<sentinel_t<_Maybe_const<_IsConst, _ViewTypes>>,
                              iterator_t<_Maybe_const<_IteratorConst, _ViewTypes>>>
                          && ...)
            _NODISCARD_FRIEND constexpr common_type_t<range_difference_t<_Maybe_const<_IteratorConst, _ViewTypes>>...>
                operator-(const _Sentinel& _Lhs, const _Iterator<_IteratorConst>& _Rhs) noexcept(
                    noexcept(-(_Rhs - _Lhs))) /* strengthened */ {
                return -(_Rhs - _Lhs);
            }
        };

        template <bool _IsConst>
        _NODISCARD static consteval bool _Is_end_noexcept() noexcept {
            if constexpr (!_Zip_is_common<_Maybe_const<_IsConst, _ViewTypes>...>) {
                return noexcept(_Sentinel<_IsConst>{
                    _Tuple_transform(_RANGES end, _STD declval<_Maybe_const<_IsConst, tuple<_ViewTypes...>>&>())});
            } else if constexpr ((random_access_range<_Maybe_const<_IsConst, _ViewTypes>> && ...)) {
                // Operations on iter_difference_t values must be noexcept, so we only
                // need to check the noexcept of begin().
                return noexcept(_STD declval<_Maybe_const<_IsConst, zip_view>&>().begin());
            } else {
                return noexcept(_Iterator<_IsConst>{
                    _Tuple_transform(_RANGES end, _STD declval<_Maybe_const<_IsConst, tuple<_ViewTypes...>>&>())});
            }
        }

    public:
        zip_view() noexcept((is_nothrow_default_constructible_v<_ViewTypes> && ...)) = default;

        constexpr explicit zip_view(_ViewTypes... _Args) noexcept(
            (is_nothrow_move_constructible_v<_ViewTypes> && ...)) // strengthened
            : _Views(_STD move(_Args)...) {}

        _NODISCARD constexpr auto begin() noexcept(
            noexcept(_Iterator<false>{_Tuple_transform(_RANGES begin, _Views)})) // strengthened
            requires (!(_Simple_view<_ViewTypes> && ...))
        {
            return _Iterator<false>{_Tuple_transform(_RANGES begin, _Views)};
        }

        _NODISCARD constexpr auto begin() const
            noexcept(noexcept(_Iterator<true>{_Tuple_transform(_RANGES begin, _Views)})) // strengthened
            requires (range<const _ViewTypes> && ...)
        {
            return _Iterator<true>{_Tuple_transform(_RANGES begin, _Views)};
        }

        _NODISCARD constexpr auto end() noexcept(_Is_end_noexcept<false>()) // strengthened
            requires (!(_Simple_view<_ViewTypes> && ...))
        {
            if constexpr (!_Zip_is_common<_ViewTypes...>) {
                return _Sentinel<false>{_Tuple_transform(_RANGES end, _Views)};
            } else if constexpr ((random_access_range<_ViewTypes> && ...)) {
                return begin() + static_cast<iter_difference_t<_Iterator<false>>>(size());
            } else {
                return _Iterator<false>{_Tuple_transform(_RANGES end, _Views)};
            }
        }

        _NODISCARD constexpr auto end() const noexcept(_Is_end_noexcept<true>()) // strengthened
            requires (range<const _ViewTypes> && ...)
        {
            if constexpr (!_Zip_is_common<const _ViewTypes...>) {
                return _Sentinel<true>{_Tuple_transform(_RANGES end, _Views)};
            } else if constexpr ((random_access_range<const _ViewTypes> && ...)) {
                return begin() + static_cast<iter_difference_t<_Iterator<true>>>(size());
            } else {
                return _Iterator<true>{_Tuple_transform(_RANGES end, _Views)};
            }
        }

        _NODISCARD constexpr auto size() noexcept(
            noexcept(_STD apply(_Size_closure(), _Tuple_transform(_RANGES size, _Views)))) // strengthened
            requires (sized_range<_ViewTypes> && ...)
        {
            return _STD apply(_Size_closure(), _Tuple_transform(_RANGES size, _Views));
        }

        _NODISCARD constexpr auto size() const
            noexcept(noexcept(_STD apply(_Size_closure(), _Tuple_transform(_RANGES size, _Views)))) // strengthened
            requires (sized_range<const _ViewTypes> && ...)
        {
            return _STD apply(_Size_closure(), _Tuple_transform(_RANGES size, _Views));
        }

    private:
        _NODISCARD static constexpr auto _Size_closure() noexcept {
            return [](auto... _Sizes) _STATIC_CALL_OPERATOR noexcept {
                using _Common_unsigned_type = _Make_unsigned_like_t<common_type_t<decltype(_Sizes)...>>;
                return (_RANGES min)({static_cast<_Common_unsigned_type>(_Sizes)...});
            };
        }
    };

    template <class... _RangeTypes>
    zip_view(_RangeTypes&&...) -> zip_view<views::all_t<_RangeTypes>...>;

    template <class... _ViewTypes>
    constexpr bool enable_borrowed_range<zip_view<_ViewTypes...>> = (enable_borrowed_range<_ViewTypes> && ...);

#if _HAS_CXX23
    template <class... _Rngs>
    _NODISCARD consteval auto _Zip_view_compile_time_max_size() {
        using _Size_type = common_type_t<decltype(_Compile_time_max_size<_Rngs>)...>;
        return (_STD min)({static_cast<_Size_type>(_Compile_time_max_size<_Rngs>)...});
    }

    template <class... _Rngs>
    constexpr auto _Compile_time_max_size<zip_view<_Rngs...>> = _Zip_view_compile_time_max_size<_Rngs...>();

    template <class... _Rngs>
    constexpr auto _Compile_time_max_size<const zip_view<_Rngs...>> = _Zip_view_compile_time_max_size<const _Rngs...>();
#endif // _HAS_CXX23

    namespace views {
        struct _Zip_fn {
        private:
            template <class... _Types>
            _NODISCARD static consteval bool _Is_invocation_noexcept() {
                if constexpr (sizeof...(_Types) == 0) {
                    // NOTE: views::empty<tuple<>> is nothrow copy-constructible.
                    return true;
                } else {
                    return noexcept(zip_view<views::all_t<_Types>...>(_STD declval<_Types>()...));
                }
            }

        public:
            template <class... _Types>
            _NODISCARD _STATIC_CALL_OPERATOR constexpr auto operator()(_Types&&... _Args) _CONST_CALL_OPERATOR
                noexcept(_Is_invocation_noexcept<_Types...>())
                requires (sizeof...(_Types) == 0)
                      || requires { zip_view<views::all_t<_Types>...>(_STD forward<_Types>(_Args)...); }
            {
                if constexpr (sizeof...(_Types) == 0) {
                    return empty_view<tuple<>>{};
                } else {
                    return zip_view<views::all_t<_Types>...>(_STD forward<_Types>(_Args)...);
                }
            }
        };
