#pragma once

#include "AudioClipAssetManager.h"
#include "ConcaveColliderAssetManager.h"
#include "config/Config.h"
#include "ConvexHullAssetManager.h"

namespace nc
{
struct AssetServices
{
    AssetServices(const config::AssetSettings& assetSettings)
        : concaveManager{assetSettings.concaveCollidersPath},
          hullManager{assetSettings.hullCollidersPath},
          audioClipManager{assetSettings.audioClipsPath}
    {
    }

    ConcaveColliderAssetManager concaveManager;
    ConvexHullAssetManager hullManager;
    AudioClipAssetManager audioClipManager;
};
} // namespace nc