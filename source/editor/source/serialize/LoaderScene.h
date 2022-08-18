#pragma once

#include "assets/AssetManifest.h"

#include <filesystem>

namespace nc::editor
{
    void WriteLoaderScene(const std::filesystem::path& projectDirectory, const AssetManifest& manifest);
}