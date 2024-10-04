#pragma once

#include "detail/EnumerateDetail.h"

#include <algorithm>
#include <type_traits>
#include <vector>

namespace nc::algo
{
/** @brief Wrapper around std::transform that returns transformed data in a new vector. */
template<class T, std::invocable<const T&> UnaryOperation>
auto Transform(const std::vector<T>& container, UnaryOperation op)
{
    using transformed_t = std::remove_cvref_t<std::invoke_result_t<UnaryOperation, const T&>>;
    auto out = std::vector<transformed_t>{};
    out.reserve(container.size());
    std::transform(std::cbegin(container), std::cend(container), std::back_inserter(out), op);
    return out;
}

/** @brief Implementation of std::enumerate. Obtain a view of [index, value] pairs from a range. */
inline detail::enumerate_view_fn Enumerate;
} // namespace nc::algo
