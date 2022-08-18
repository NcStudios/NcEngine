#pragma once

#include "AudioClipAssetManager.h"
#include "ConcaveColliderAssetManager.h"
#include "config/Config.h"
#include "ConvexHullAssetManager.h"
#include "CubeMapAssetManager.h"

namespace nc
{
struct AssetServices
{
    AssetServices(graphics::Graphics* graphics, const config::AssetSettings& assetSettings, uint32_t maxTextures)
        : concaveManager{assetSettings.concaveCollidersPath},
          hullManager{assetSettings.hullCollidersPath},
          audioClipManager{assetSettings.audioClipsPath},
          cubeMapAssetManager{graphics, assetSettings.cubeMapsPath, maxTextures}
    {
    }

    ConcaveColliderAssetManager concaveManager;
    ConvexHullAssetManager hullManager;
    AudioClipAssetManager audioClipManager;
    CubeMapAssetManager cubeMapAssetManager;
};
} // namespace nc