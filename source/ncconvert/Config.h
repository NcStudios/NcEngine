#pragma once

#include "ncasset/AssetType.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace nc::convert
{
/** @brief Identifies different modes of operation for nc-convert. */
enum class OperationMode
{
    /** @brief Default value for failure cases. */
    Unspecified,

    /** @brief Perform a single conversion from command line arguments. */
    SingleTarget,

    /** @brief Perform conversions from a manifest file. */
    Manifest,

    /** @brief Print details of an existing .nca file. */
    Inspect
};

/** @brief Build controls generated from command line options. */
struct Config
{
    /** @brief The operation to be performed. */
    OperationMode mode = OperationMode::Unspecified;

    /** @brief The directory to output .nca files to. */
    std::filesystem::path outputDirectory = "./";

    /**
     * @brief A path to a raw asset file to be converted to .nca format.
     * @note Specific to single target and inspect modes
     */
    std::optional<std::filesystem::path> targetPath;

    /**
     * @brief The name of the output .nca file.
     * @note Specific to single target mode
     */
    std::optional<std::string> assetName;

    /**
     * @brief The type of the asset to convert.
     * @note Specific to single target mode.
     */
    std::optional<asset::AssetType> targetType;

    /**
     * @brief A path to a manifest containing .nca conversion instructions.
     * @note Specific to manifest mode.
     */
    std::optional<std::filesystem::path> manifestPath;
};
} // namespace nc::convert
