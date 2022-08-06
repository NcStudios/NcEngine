#pragma once

#include "AudioClipAssetManager.h"
#include "ConcaveColliderAssetManager.h"
#include "ConvexHullAssetManager.h"
#include "CubeMapAssetManager.h"
#include "TextureAssetManager.h"
#include "config/Config.h"

namespace nc
{
struct AssetServices
{
    AssetServices(graphics::Graphics* graphics, const config::AssetSettings& assetSettings, uint32_t maxTextures)
        : concaveManager{assetSettings.concaveCollidersPath},
          hullManager{assetSettings.hullCollidersPath},
          audioClipManager{assetSettings.audioClipsPath},
          textureManager{graphics, assetSettings.texturesPath, maxTextures},
          cubeMapAssetManager{graphics, assetSettings.cubeMapsPath, maxTextures}
    {
    }

    ConcaveColliderAssetManager concaveManager;
    ConvexHullAssetManager hullManager;
    AudioClipAssetManager audioClipManager;
    TextureAssetManager textureManager;
    CubeMapAssetManager cubeMapAssetManager;
};
}