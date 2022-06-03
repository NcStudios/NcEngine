#include "assets/Assets.h"
#include "graphics/AssetsSink.h"
#include "MeshAssetManager.h"

namespace nc
{
        Assets::Assets(graphics::AssetsSink* assetsSink, const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings)
            : meshManager{std::make_unique<MeshAssetManager>(assetsSink, projectSettings.meshesPath)}
        {
        }

        Assets::~Assets() {}
}