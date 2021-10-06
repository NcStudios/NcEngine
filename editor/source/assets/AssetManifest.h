#pragma once

#include "AssetCollection.h"

namespace nc::editor
{
    /** A collection of all assets required in a project. */
    class AssetManifest
    {
        public:
            AssetManifest(const std::filesystem::path& projectDirectory);

            bool Add(const std::filesystem::path& assetPath, AssetType type);
            bool Remove(const std::filesystem::path& assetPath, AssetType type);
            bool Contains(const std::filesystem::path& assetPath, AssetType type) const;
            auto View(AssetType type) const -> std::span<const Asset>;

            void Read(const std::filesystem::path& projectDirectory);
            void Write(const std::filesystem::path& projectDirectory) const;

        private:
            AssetCollection m_meshes;
            AssetCollection m_hullColliders;
            AssetCollection m_concaveColliders;
            AssetCollection m_textures;
            AssetCollection m_audioClips;

            auto GetCollection(AssetType type) -> AssetCollection&;
            auto GetCollection(AssetType type) const -> const AssetCollection&;
    };
}