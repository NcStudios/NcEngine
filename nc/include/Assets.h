#pragma once

#include <filesystem>

namespace nc
{
    void LoadSoundClipAsset(const std::filesystem::path& path);
    void LoadConvexHullAsset(const std::filesystem::path& path);
    void LoadConcaveColliderAsset(const std::filesystem::path& path);
}