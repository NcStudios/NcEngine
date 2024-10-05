#pragma once

#include <filesystem>
#include <optional>

namespace nc::convert
{
/** @brief Data describing a required asset conversion. */
struct Target
{
    Target(std::filesystem::path source,
           std::filesystem::path destination,
           std::optional<std::string> subResource = std::nullopt)
        : sourcePath{std::move(source)},
          destinationPath{std::move(destination)},
          subResourceName{std::move(subResource)}
    {
    }

    std::filesystem::path sourcePath;
    std::filesystem::path destinationPath;
    std::optional<std::string> subResourceName;
};
}
