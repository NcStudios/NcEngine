#pragma once

#include "ncengine/ecs/Entity.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <concepts>
#include <vector>

/** @cond internal */
namespace nc::ecs::detail
{
template<class T>
struct StagedComponent
{
    Entity entity;
    T component;

    auto operator==(const Entity& other) const noexcept
    {
        return entity == other;
    }
};

template<class T, class... Args>
auto Construct(Entity entity, Args&&... args)
{
    if constexpr (std::constructible_from<T, Entity, Args...>)
        return T{entity, std::forward<Args>(args)...};
    else
        return T{std::forward<Args>(args)...};
}

template<class T, class U>
auto EraseUnstable(std::vector<T>& container, const U& value) -> bool
{
    auto pos = std::find(container.begin(), container.end(), value);
    if (pos != container.end())
    {
        *pos = std::move(container.back());
        container.pop_back();
        return true;
    }

    return false;
}
} // namespace nc::ecs::detail
/** @endcond */
