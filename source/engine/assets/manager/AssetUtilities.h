#pragma once

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nc
{
auto HasValidAssetExtension(const std::string& path) -> bool;

template<class T>
auto GetPaths(const std::unordered_map<std::string, T>& map) -> std::vector<std::string_view>
{
    auto out = std::vector<std::string_view>{};
    out.reserve(map.size());
    std::transform(map.cbegin(), map.cend(), std::back_inserter(out), [](const auto& pair)
    {
        return std::string_view{pair.first};
    });

    return out;
}

inline auto GetPaths(const std::vector<std::string>& vec) -> std::vector<std::string_view>
{
    auto out = std::vector<std::string_view>{};
    out.reserve(vec.size());
    std::transform(vec.cbegin(), vec.cend(), std::back_inserter(out), [](const auto& str)
    {
        return std::string_view{str};
    });

    return out;
}

template<class T, class Proj>
auto GetPaths(const std::vector<T>& vec, Proj&& proj) -> std::vector<std::string_view>
{
    auto out = std::vector<std::string_view>{};
    out.reserve(vec.size());
    std::transform(vec.cbegin(), vec.cend(), std::back_inserter(out), [&proj](const auto& item)
    {
        return std::string_view{proj(item)};
    });

    return out;
}
} // namespace nc
