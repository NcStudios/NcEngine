#include "AssetManifest.h"
#include "ManifestSerializer.h"
#include "utility/DefaultComponents.h"
#include "utility/Output.h"

namespace
{
    bool IsDefaultAsset(const std::filesystem::path& path, nc::editor::AssetType type)
    {
        switch(type)
        {
            case nc::editor::AssetType::AudioClip:
            {
                /** @todo */
                return false;
            }
            case nc::editor::AssetType::ConcaveCollider:
            {
                return path.string() == nc::editor::PlaneConcaveColliderPath;
            }
            case nc::editor::AssetType::HullCollider:
            {
                return path.string() == nc::editor::CubeHullColliderPath;
            }
            case nc::editor::AssetType::Mesh:
            {
                auto str = path.string();
                return str == nc::editor::CubeMeshPath ||
                       str == nc::editor::CapsuleMeshPath ||
                       str == nc::editor::SphereMeshPath ||
                       str == nc::editor::PlaneMeshPath;
            }
            case nc::editor::AssetType::Texture:
            {
                auto str = path.string();
                return str == nc::editor::DefaultBaseColorPath ||
                       str == nc::editor::DefaultNormalPath ||
                       str == nc::editor::DefaultRoughnessPath;
            }
        }

        return false;
    }
}

namespace nc::editor
{
    AssetManifest::AssetManifest(const std::filesystem::path& projectDirectory)
        : m_meshes{},
          m_hullColliders{},
          m_concaveColliders{},
          m_textures{},
          m_audioClips{}
    {
        LoadDefaultAssets();
        Read(projectDirectory);
    }

    bool AssetManifest::Add(const std::filesystem::path& assetPath, AssetType type)
    {
        if(!HasValidExtensionForAssetType(assetPath, type))
        {
            Output::LogError("Invalid asset extension");
            return false;
        }

        auto& collection = GetCollection(type);

        if(collection.Contains(assetPath))
        {
            Output::LogError("Asset is alread in the manifest");
            return false;
        }

        auto asset = CreateAsset(assetPath, type);

        if(RequiresNcaFile(type) && !BuildNcaFile(assetPath, type))
        {
            Output::LogError("Failure building nca file from:", assetPath.string());
            return false;
        }

        if(!LoadAsset(asset, type))
        {
            Output::LogError("Failure loading asset:", assetPath.string());
            return false;
        }

        collection.Add(std::move(asset));
        Output::Log("Added asset: " + assetPath.string());
        return true;
    }

    bool AssetManifest::Remove(const std::filesystem::path& assetPath, AssetType type)
    {
        /** @todo unload assets once implemented */
        Output::Log("Removed asset: " + assetPath.string());
        return GetCollection(type).Remove(assetPath);
    }

    bool AssetManifest::Contains(const std::filesystem::path& assetPath, AssetType type) const
    {
        return GetCollection(type).Contains(assetPath) || IsDefaultAsset(assetPath, type);
    }

    bool AssetManifest::ContainsNca(const std::filesystem::path& assetPath, AssetType type) const
    {
        return GetCollection(type).ContainsNca(assetPath) || IsDefaultAsset(assetPath, type);
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
            Output::LogError("Failure reading manifest:", manifestPath.string());
            return;
        }

        for(auto& asset : manifestData.audioClips)
        {
            if(!LoadAsset(asset, AssetType::AudioClip))
            {
                Output::LogError("Failure loading AudioClip:",  asset.sourcePath.string());
            }

            m_audioClips.Add(std::move(asset));
        }

        for(auto& asset : manifestData.concaveColliders)
        {
            if(!LoadAsset(asset, AssetType::ConcaveCollider))
                Output::LogError("Failure loading ConcaveCollider:", asset.sourcePath.string());
            
            m_concaveColliders.Add(std::move(asset));
        }

        for(auto& asset : manifestData.hullColliders)
        {
            if(!LoadAsset(asset, AssetType::HullCollider))
                Output::LogError("Failure loading HullCollider:", asset.sourcePath.string());

            m_hullColliders.Add(std::move(asset));
        }

        for(auto& asset : manifestData.meshes)
        {
            if(!LoadAsset(asset, AssetType::Mesh))
                Output::LogError("Failure loading Mesh:", asset.sourcePath.string());
            
            m_meshes.Add(std::move(asset));
        }

        for(auto& asset : manifestData.textures)
        {
            if(!LoadAsset(asset, AssetType::Texture))
                Output::LogError("Failure loading Texture:", asset.sourcePath.string());
            
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