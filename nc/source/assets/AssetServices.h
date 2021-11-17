#pragma once

#include "AudioClipAssetManager.h"
#include "ConcaveColliderAssetManager.h"
#include "ConvexHullAssetManager.h"
#include "MeshAssetManager.h"
#include "TextureAssetManager.h"
#include "config/Config.h"

namespace nc
{
    struct AssetServices
    {
        AssetServices(graphics::Graphics* graphics, const config::ProjectSettings& projectSettings, uint32_t maxTextures)
            : concaveManager{projectSettings.concaveCollidersPath},
              hullManager{projectSettings.hullCollidersPath},
              meshManager{graphics, projectSettings.meshesPath},
              audioClipManager{projectSettings.audioClipsPath},
              textureManager{graphics, projectSettings.texturesPath, maxTextures}
        {
        }

        ConcaveColliderAssetManager concaveManager;
        ConvexHullAssetManager hullManager;
        MeshAssetManager meshManager;
        AudioClipAssetManager audioClipManager;
        TextureAssetManager textureManager;
    };
}