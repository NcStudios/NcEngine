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
    AssetServices(graphics::Graphics* graphics, const config::ProjectSettings& projectSettings, uint32_t maxTextures)
        : concaveManager{projectSettings.concaveCollidersPath},
            hullManager{projectSettings.hullCollidersPath},
            audioClipManager{projectSettings.audioClipsPath},
            textureManager{graphics, projectSettings.texturesPath, maxTextures},
            cubeMapAssetManager{graphics, projectSettings.cubeMapsPath, maxTextures}
    {
    }

    ConcaveColliderAssetManager concaveManager;
    ConvexHullAssetManager hullManager;
    AudioClipAssetManager audioClipManager;
    TextureAssetManager textureManager;
    CubeMapAssetManager cubeMapAssetManager;
};
}