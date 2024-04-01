/**
 * @file EcsFilter.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <type_traits>

namespace nc::ecs
{
/**
 * @brief Filter option for AccessPolicy providing coarse control over type access.
 * @internal Values are required to be ordered by increasing permissiveness.
*/
enum class FilterBase
{
    None  = 0, // Include only explicitly listed types and those derived from FreeComponent.
    Basic = 1, // Include common types needed to enable operations and any explicitly requested types.
    All   = 2  // Include all types.
};

/** @cond internal */
namespace detail
{
template<class... Ts>
struct MatchFilter
{
    template<class T>
    static constexpr auto included = (std::is_same_v<T, Ts> || ...);
};

struct AllFilter
{
    template<class T>
    static constexpr auto included = true;
};
} // namespace detail
/** @endcond */
} // namespace nc::ecs
