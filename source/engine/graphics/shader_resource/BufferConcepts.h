#pragma once

#include <ranges>
#include <type_traits>

namespace nc::graphics
{
template<class Rng>
concept TriviallyCopyableRange =
    std::ranges::contiguous_range<Rng> &&
    std::ranges::sized_range<Rng> &&
    std::is_trivially_copyable_v<std::ranges::range_value_t<Rng>>;
} // namespace nc::graphics
