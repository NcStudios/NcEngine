/**
 * @file EnumUtilities.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <type_traits>

namespace nc::utility
{
    /**
     * @brief Convert an enum value to its underlying type.
     */
    template<class T>
        requires std::is_enum_v<T>
    constexpr auto to_underlying(T t) -> std::underlying_type_t<T>
    {
        return static_cast<std::underlying_type_t<T>>(t);
    }
} // namespace nc::utility

/** Define operators for an enum. */
#define DEFINE_BITWISE_OPERATORS(type)                                                                    \
constexpr type operator|(type l, type r)                                                                  \
{                                                                                                         \
    return static_cast<type>(nc::utility::to_underlying<type>(l) | nc::utility::to_underlying<type>(r));  \
}                                                                                                         \
constexpr type operator&(type l, type r)                                                                  \
{                                                                                                         \
    return static_cast<type>(nc::utility::to_underlying<type>(l) & nc::utility::to_underlying<type>(r));  \
}                                                                                                         \
constexpr type operator~(type v)                                                                          \
{                                                                                                         \
    return static_cast<type>(~nc::utility::to_underlying<type>(v));                                       \
}                                                                                                         \
constexpr type operator^(type l, type r)                                                                  \
{                                                                                                         \
    return static_cast<type>(nc::utility::to_underlying<type>(l) ^ nc::utility::to_underlying<type>(r));  \
}                                                                                                         \
constexpr type operator<<(type l, type r)                                                                 \
{                                                                                                         \
    return static_cast<type>(nc::utility::to_underlying<type>(l) << nc::utility::to_underlying<type>(r)); \
}                                                                                                         \
constexpr type operator>>(type l, type r)                                                                 \
{                                                                                                         \
    return static_cast<type>(nc::utility::to_underlying<type>(l) >> nc::utility::to_underlying<type>(r)); \
}
