#pragma once

#include "AssetType.h"

#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace nc::editor
{
    struct Asset
    {
        std::string name;
        std::filesystem::path sourcePath;
        std::optional<std::filesystem::path> ncaPath;
        std::filesystem::file_time_type lastWriteTime;
    };

    /** A collection of all assets required in a project. */
    class AssetManifest
    {
        public:
            template<AssetType Type>
            bool Add(const std::filesystem::path& assetPath);

            bool Add(const std::filesystem::path& assetPath, AssetType type);

            template<AssetType Type>
            void Remove(const std::filesystem::path& assetPath);

            void Remove(const std::filesystem::path& assetPath, AssetType type);

            template<AssetType Type>
            bool Contains(const std::filesystem::path& assetPath) const;

            bool Contains(const std::filesystem::path& assetPath, AssetType type) const;

            template<AssetType type>
            auto GetAll() const -> std::span<const Asset>;

            auto GetAll(AssetType type) const -> std::span<const Asset>;

            void ReadManifest(const std::filesystem::path& projectDirectory);
            void WriteManifest(const std::filesystem::path& projectDirectory) const;

        private:
            std::vector<Asset> m_meshes;
            std::vector<Asset> m_hullColliders;
            std::vector<Asset> m_concaveColliders;
            std::vector<Asset> m_textures;
            std::vector<Asset> m_audioClips;
    };

    template<> bool AssetManifest::Add<AssetType::AudioClip>(const std::filesystem::path&);
    template<> bool AssetManifest::Add<AssetType::ConcaveCollider>(const std::filesystem::path&);
    template<> bool AssetManifest::Add<AssetType::HullCollider>(const std::filesystem::path&);
    template<> bool AssetManifest::Add<AssetType::Mesh>(const std::filesystem::path&);
    template<> bool AssetManifest::Add<AssetType::Texture>(const std::filesystem::path&);

    template<> void AssetManifest::Remove<AssetType::AudioClip>(const std::filesystem::path&);
    template<> void AssetManifest::Remove<AssetType::ConcaveCollider>(const std::filesystem::path&);
    template<> void AssetManifest::Remove<AssetType::HullCollider>(const std::filesystem::path&);
    template<> void AssetManifest::Remove<AssetType::Mesh>(const std::filesystem::path&);
    template<> void AssetManifest::Remove<AssetType::Texture>(const std::filesystem::path&);

    template<> bool AssetManifest::Contains<AssetType::AudioClip>(const std::filesystem::path&) const;
    template<> bool AssetManifest::Contains<AssetType::ConcaveCollider>(const std::filesystem::path&) const;
    template<> bool AssetManifest::Contains<AssetType::HullCollider>(const std::filesystem::path&) const;
    template<> bool AssetManifest::Contains<AssetType::Mesh>(const std::filesystem::path&) const;
    template<> bool AssetManifest::Contains<AssetType::Texture>(const std::filesystem::path&) const;

    template<> auto AssetManifest::GetAll<AssetType::AudioClip>() const -> std::span<const Asset>;
    template<> auto AssetManifest::GetAll<AssetType::ConcaveCollider>() const -> std::span<const Asset>;
    template<> auto AssetManifest::GetAll<AssetType::HullCollider>() const -> std::span<const Asset>;
    template<> auto AssetManifest::GetAll<AssetType::Mesh>() const -> std::span<const Asset>;
    template<> auto AssetManifest::GetAll<AssetType::Texture>() const -> std::span<const Asset>;
}