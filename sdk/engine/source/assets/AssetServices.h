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
    AssetServices(graphics::Graphics* graphics, const config::ProjectSettings& projectSettings, uint32_t maxTextures)
        : concaveManager{projectSettings.concaveCollidersPath},
          hullManager{projectSettings.hullCollidersPath},
          audioClipManager{projectSettings.audioClipsPath},
          cubeMapAssetManager{graphics, projectSettings.cubeMapsPath, maxTextures}
    {
    }

    ConcaveColliderAssetManager concaveManager;
    ConvexHullAssetManager hullManager;
    AudioClipAssetManager audioClipManager;
    CubeMapAssetManager cubeMapAssetManager;
};
} // namespace nc