#pragma once

#include "ConcaveColliderAssetManager.h"
#include "ConvexHullAssetManager.h"
#include "MeshAssetManager.h"
#include "SoundClipAssetManager.h"
#include "TextureAssetManager.h"

namespace nc
{
    struct AssetServices
    {
        AssetServices(graphics::Graphics* graphics, uint32_t maxTextures)
            : concaveManager{},
              hullManager{},
              meshManager{graphics},
              soundClipManager{},
              textureManager{graphics, maxTextures}
        {
        }

        ConcaveColliderAssetManager concaveManager;
        ConvexHullAssetManager hullManager;
        MeshAssetManager meshManager;
        SoundClipAssetManager soundClipManager;
        TextureAssetManager textureManager;
    };
}