#include "assets/Assets.h"
#include "AudioClipAssetManager.h"
#include "ConcaveColliderAssetManager.h"
#include "ConvexHullAssetManager.h"
#include "CubeMapAssetManager.h"
#include "graphics/Graphics.h"
#include "MeshAssetManager.h"
#include "TextureAssetManager.h"

namespace nc
{
        Assets::Assets(graphics::Graphics* graphics, const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings)
            : concaveManager{std::make_unique<ConcaveColliderAssetManager>(projectSettings.concaveCollidersPath)},
              hullManager{std::make_unique<ConvexHullAssetManager>(projectSettings.hullCollidersPath)},
              audioClipManager{std::make_unique<AudioClipAssetManager>(projectSettings.audioClipsPath)},
              meshManager{std::make_unique<MeshAssetManager>(graphics, projectSettings.meshesPath)}, // Graphics
              textureManager{std::make_unique<TextureAssetManager>(graphics, projectSettings.texturesPath, memorySettings.maxTextures)}, // Graphics
              cubeMapAssetManager{std::make_unique<CubeMapAssetManager>(graphics, projectSettings.cubeMapsPath, memorySettings.maxTextures)} // Graphics
        {
        }

        Assets::~Assets() {}
}