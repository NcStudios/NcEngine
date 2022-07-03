#include "assets/Assets.h"
#include "MeshAssetManager.h"

namespace nc
{
        Assets::Assets(const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings)
            : meshManager{std::make_unique<MeshAssetManager>(projectSettings.meshesPath)}
        {
        }

        Assets::~Assets() {}
}