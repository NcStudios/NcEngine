#pragma once

#include <filesystem>

namespace nc::convert
{
/** @brief Print details about an asset. */
void Inspect(const std::filesystem::path& ncaPath);
} // namespace nc::convert
