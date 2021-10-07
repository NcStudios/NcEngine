#include "AssetManifest.h"
#include "ManifestSerializer.h"
#include "utility/Output.h"


#include <iostream>

namespace nc::editor
{
    AssetManifest::AssetManifest(const std::filesystem::path& projectDirectory)
        : m_meshes{},
          m_hullColliders{},
          m_concaveColliders{},
          m_textures{},
          m_audioClips{}
    {
        Read(projectDirectory);
    }

    bool AssetManifest::Add(const std::filesystem::path& assetPath, AssetType type)
    {
        if(!HasValidExtensionForAssetType(assetPath, type))
        {
            Output::Log("AssetManifest::Add - Invalid file extension for this asset type: " + assetPath.string());
            return false;
        }

        auto& collection = GetCollection(type);

        if(collection.Contains(assetPath))
        {
            Output::Log("AssetManifest::Add - Manifest already has asset: " + assetPath.string());
            return false;
        }

        auto asset = CreateAsset(assetPath, type);

        if(RequiresNcaFile(type) && !BuildNcaFile(assetPath, type))
        {
            Output::Log("AssetManifest::Add - Failure building nca file from: " + assetPath.string());
            return false;
        }

        if(!LoadAsset(asset, type))
        {
            /** @todo If we fail to load, should we delete the nca file? (if it exists) */
            Output::Log("AssetManifest::Add - Failure loading asset: " + assetPath.string());
            return false;
        }

        collection.Add(std::move(asset));
        return true;
    }

    bool AssetManifest::Remove(const std::filesystem::path& assetPath, AssetType type)
    {
        /** @todo unload assets once implemented */
        return GetCollection(type).Remove(assetPath);
    }

    bool AssetManifest::Contains(const std::filesystem::path& assetPath, AssetType type) const
    {
        return GetCollection(type).Contains(assetPath);
    }

    auto AssetManifest::View(AssetType type) const -> std::span<const Asset>
    {
        return GetCollection(type).View();
    }

    void AssetManifest::Read(const std::filesystem::path& projectDirectory)
    {
        auto manifestPath = projectDirectory / "assets/manifest.txt";
        auto manifestData = ReadManifest(manifestPath);

        if(!manifestData.readSuccessfully)
        {
            Output::Log("AssetManifest::ReadManifest - Failure reading manifest: " + manifestPath.string());
            return;
        }

        for(auto& asset : manifestData.audioClips)
        {
            if(!LoadAsset(asset, AssetType::AudioClip))
            {
                Output::Log("AssetManifest::ReadManifest - Failure loading AudioClip: " + asset.sourcePath.string());
            }

            m_audioClips.Add(std::move(asset));
        }

        for(auto& asset : manifestData.concaveColliders)
        {
            if(!LoadAsset(asset, AssetType::ConcaveCollider))
                Output::Log("AssetManifest::ReadManifest - Failure loading ConcaveCollider: " + asset.sourcePath.string());
            
            m_concaveColliders.Add(std::move(asset));
        }

        for(auto& asset : manifestData.hullColliders)
        {
            if(!LoadAsset(asset, AssetType::HullCollider))
                Output::Log("AssetManifest::ReadManifest - Failure loading HullCollider: " + asset.sourcePath.string());

            m_hullColliders.Add(std::move(asset));
        }

        /** @todo add once fixed */
        // for(auto& asset : manifestData.meshes) {}

        for(auto& asset : manifestData.textures)
        {
            if(!LoadAsset(asset, AssetType::Texture))
                Output::Log("AssetManifest::ReadManifest - Failure loading Texture: " + asset.sourcePath.string());
            
            m_textures.Add(std::move(asset));
        }
    }

    void AssetManifest::Write(const std::filesystem::path& projectDirectory) const
    {
        auto manifestPath = projectDirectory / "assets/manifest.txt";
        WriteManifest(manifestPath, *this);
    }

    auto AssetManifest::GetCollection(AssetType type) -> AssetCollection&
    {
        switch(type)
        {
            case AssetType::AudioClip:       return m_audioClips;
            case AssetType::ConcaveCollider: return m_concaveColliders;
            case AssetType::HullCollider:    return m_hullColliders;
            case AssetType::Mesh:            return m_meshes;
            case AssetType::Texture:         return m_textures;
        }

        throw std::runtime_error("AssetManifest::GetCollection - Unknown AssetType");
    }

    auto AssetManifest::GetCollection(AssetType type) const -> const AssetCollection&
    {
        switch(type)
        {
            case AssetType::AudioClip:       return m_audioClips;
            case AssetType::ConcaveCollider: return m_concaveColliders;
            case AssetType::HullCollider:    return m_hullColliders;
            case AssetType::Mesh:            return m_meshes;
            case AssetType::Texture:         return m_textures;
        }

        throw std::runtime_error("AssetManifest::GetCollection - Unknown AssetType");
    }
}