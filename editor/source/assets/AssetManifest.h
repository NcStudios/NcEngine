#pragma once

#include "AssetUtilities.h"
#include "AssetCollection.h"
#include "framework/Callbacks.h"

namespace nc::editor
{
    /** A collection of all assets required in a project. */
    class AssetManifest
    {
        public:
            AssetManifest(const std::filesystem::path& projectDirectory, GetProjectConfigCallbackType configCallback);

            bool Add(const std::filesystem::path& assetPath, AssetType type);
            bool AddSkybox(const CubeMapFaces& assetPaths, const std::string& name);
            bool AddSkybox(const std::string& name);
            bool EditSkybox(const CubeMapFaces& previousPaths, const CubeMapFaces& newPaths, const std::string& name);
            bool Remove(const std::filesystem::path& assetPath, AssetType type);
            bool Contains(const std::filesystem::path& assetPath, AssetType type) const;
            bool ContainsNca(const std::filesystem::path& assetPath, AssetType type) const;
            auto View(AssetType type) const -> std::span<const Asset>;

            void Read(const std::filesystem::path& projectDirectory);
            void Write(const std::filesystem::path& projectDirectory) const;

        private:
            AssetCollection m_meshes;
            AssetCollection m_hullColliders;
            AssetCollection m_concaveColliders;
            AssetCollection m_textures;
            AssetCollection m_audioClips;
            AssetCollection m_skyboxes;

            GetProjectConfigCallbackType m_getConfig;

            auto GetCollection(AssetType type) -> AssetCollection&;
            auto GetCollection(AssetType type) const -> const AssetCollection&;
            auto GetAssetImportPath(const std::filesystem::path& assetPath, AssetType type) const -> std::filesystem::path;
    };
}