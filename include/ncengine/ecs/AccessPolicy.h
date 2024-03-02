/**
 * @file AccessPolicy.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include "ncengine/ecs/ComponentRegistry.h"
#include "ncengine/ecs/Hierarchy.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"

#include <concepts>
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

/** @brief Specifies a type satisfies the requirements to be managed by the ComponentRegistry. */
template<class T>
concept RegistryType = Component<T> || std::same_as<Entity, T>;

/** @brief Specifies a type is invocable with a data pool of type T. */
template<class F, class T>
concept PoolInvocable = std::invocable<F, decltype(std::declval<ComponentRegistry*>()->GetPool<T>())>;

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

/**
 * @brief A proxy for ComponentRegistry providing filtered access to data pools.
 * @tparam Base A FilterBase value indicating a baseline set of types to be added to the policy.
 * @tparam Includes A list types to include in addition to the base set.
 */
template<FilterBase Base, class... Includes>
class AccessPolicy
{
    public:
        /** @brief The value of the policy's FilterBase. */
        static constexpr auto base = Base;

        /** @brief The policy's filter type. */
        using FilterType = std::conditional_t<Base == FilterBase::All,
            detail::AllFilter,
            std::conditional_t<Base == FilterBase::Basic,
                detail::MatchFilter<Entity, Hierarchy, Tag, Transform, detail::FreeComponentGroup, Includes...>,
                detail::MatchFilter<detail::FreeComponentGroup, Includes...>>>;

        /** @brief Indicates whether all requested types are accessible by the policy. */
        template<class... TargetIncludes>
        static constexpr bool HasAccess = (FilterType::template included<TargetIncludes> && ...);

        /** @brief Indicates the policy's FilterBase is at least as permissive as the requested base. */
        template<FilterBase TargetBase>
        static constexpr bool BaseContains = Base >= TargetBase;

        /** @brief Indicates whether the policy is convertible to another policy with the requested ruleset. */
        template<FilterBase TargetBase, class... TargetIncludes>
        static constexpr bool ConvertibleTo = BaseContains<TargetBase> && HasAccess<TargetIncludes...>;

        /** @brief Construct an AccessPolicy object. */
        AccessPolicy(ComponentRegistry& registry) noexcept
            : m_registry{&registry} {}

        /** @brief Implicit conversion operator to a more restricted policy. */
        template<FilterBase TargetBase, class... TargetIncludes>
            requires ConvertibleTo<TargetBase, TargetIncludes...>
        operator AccessPolicy<TargetBase, TargetIncludes...>() const noexcept
        {
            return AccessPolicy<TargetBase, TargetIncludes...>{*m_registry};
        }

        /** @brief Get the pool for a given type. */
        template<RegistryType T>
            requires HasAccess<T>
        auto GetPool() const -> decltype(auto)
        {
            return m_registry->GetPool<T>();
        }

        /** @brief Get the pool for a given type. */
        template<RegistryType T>
            requires HasAccess<T>
        auto GetPool() -> decltype(auto)
        {
            return m_registry->GetPool<T>();
        }

        /** @brief Get a range of pointers to all ComponentPoolBase instances. */
        auto GetComponentPools()
            requires BaseContains<FilterBase::All>
        {
            return m_registry->GetComponentPools();
        }

    private:
        mutable ComponentRegistry* m_registry;
};
} // namespace nc::ecs
