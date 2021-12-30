#include "AssetManifest.h"
#include "ManifestSerializer.h"
#include "utility/DefaultComponents.h"
#include "utility/Output.h"

#include <iostream>

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
            case nc::editor::AssetType::Skybox:
            {
                return path.string() == nc::editor::DefaultSkyboxPath;
            }
        }

        return false;
    }

    void DeleteMeshFiles(const std::filesystem::path& assetPath)
    {
        auto fbxPath = assetPath;
        fbxPath.replace_extension(".fbx");

        auto objPath = assetPath;
        objPath.replace_extension(".obj");

        if (std::filesystem::exists(fbxPath)) std::filesystem::remove(fbxPath);
        if (std::filesystem::exists(assetPath)) std::filesystem::remove(assetPath);
    }
}

namespace nc::editor
{
    AssetManifest::AssetManifest(const std::filesystem::path& projectDirectory, GetProjectConfigCallbackType configCallback)
        : m_meshes{},
          m_hullColliders{},
          m_concaveColliders{},
          m_textures{},
          m_audioClips{},
          m_skyboxes{},
          m_getConfig{std::move(configCallback)}
    {
        LoadDefaultAssets();
        Read(projectDirectory);
    }

    auto AssetManifest::GetAssetImportPath(const std::filesystem::path& assetPath, AssetType type) const -> std::filesystem::path
    {
        const auto& projectSettings = m_getConfig().projectSettings;
        switch(type)
        {
            case AssetType::AudioClip:       return projectSettings.audioClipsPath / assetPath.filename();
            case AssetType::ConcaveCollider: return projectSettings.concaveCollidersPath / assetPath.filename();
            case AssetType::HullCollider:    return projectSettings.hullCollidersPath / assetPath.filename();
            case AssetType::Mesh:            return projectSettings.meshesPath / assetPath.filename();
            case AssetType::Texture:         return projectSettings.texturesPath / assetPath.filename();
            case AssetType::Skybox:          return projectSettings.cubeMapsPath / assetPath.filename();
            default: throw NcError("Unknown AssetType");
        }
    }

    bool AssetManifest::Add(const std::filesystem::path& assetPath, AssetType type)
    {
        if(!HasValidExtensionForAssetType(assetPath, type))
        {
            Output::LogError("Invalid asset extension");
            return false;
        }

        const auto& importPath = GetAssetImportPath(assetPath, type);
        auto& collection = GetCollection(type);

        if(collection.Contains(importPath))
        {
            Output::LogError("Asset is already in the manifest");
            return false;
        }

        std::filesystem::copy(assetPath, importPath);
        auto asset = CreateAsset(importPath, type);

        if(RequiresNcaFile(type) && !BuildNcaFile(importPath, type))
        {
            Output::LogError("Failure building nca file from:", importPath.string());
            return false;
        }

        if(!LoadAsset(asset, type))
        {
            Output::LogError("Failure loading asset:", importPath.string());
            return false;
        }

        collection.Add(std::move(asset));
        Output::Log("Added asset: " + importPath.string());
        return true;
    }

    /**
     * Call AssetManifest.Add, potentially overload.
     * This will copy all six files to a directory, normalizing the filenames.
     * This will then pass that directory to AssetBuilder.BuildSkyboxPath(folderName, config)
     * 
     * */
    bool AssetManifest::AddSkybox(const CubeMapFaces& assetPaths, const std::string& name)
    {
        if(!std::filesystem::exists(assetPaths.frontPath))
        {
            Output::LogError("Face path does not exist.");
            return false;
        }

        if(!std::filesystem::exists(assetPaths.backPath))
        {
            Output::LogError("Face path does not exist.");
            return false;
        }

        if(!std::filesystem::exists(assetPaths.upPath))
        {
            Output::LogError("Face path does not exist.");
            return false;
        }

        if(!std::filesystem::exists(assetPaths.downPath))
        {
            Output::LogError("Face path does not exist.");
            return false;
        }

        if(!std::filesystem::exists(assetPaths.rightPath))
        {
            Output::LogError("Face path does not exist.");
            return false;
        }

        if(!std::filesystem::exists(assetPaths.leftPath))
        {
            Output::LogError("Face path does not exist.");
            return false;
        }
     
        auto& collection = GetCollection(AssetType::Skybox);

        if(collection.Contains(name))
        {
            Output::LogError("Asset is already in the manifest");
            return false;
        }

        const auto subdirectory = std::filesystem::path(name);
        const auto& projectSettings = m_getConfig().projectSettings;

        if (!std::filesystem::exists(projectSettings.cubeMapsPath/subdirectory))
        {
            std::filesystem::create_directory(projectSettings.cubeMapsPath/subdirectory);
        }

        try
        {
            {
                const auto filePath = "front" + std::filesystem::path(assetPaths.frontPath).extension().string();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);
                if (std::filesystem::exists(importPath)) std::filesystem::remove(importPath);
                std::filesystem::copy(assetPaths.frontPath, importPath);
            }

            {
                const auto filePath = "back" + std::filesystem::path(assetPaths.backPath).extension().string();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);
                if (std::filesystem::exists(importPath)) std::filesystem::remove(importPath);
                std::filesystem::copy(assetPaths.backPath, importPath);
            }

            {
                const auto filePath = "up" + std::filesystem::path(assetPaths.upPath).extension().string();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);
                if (std::filesystem::exists(importPath)) std::filesystem::remove(importPath);
                std::filesystem::copy(assetPaths.upPath, importPath);
            }

            {
                const auto filePath = "down" + std::filesystem::path(assetPaths.downPath).extension().string();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);
                if (std::filesystem::exists(importPath)) std::filesystem::remove(importPath);
                std::filesystem::copy(assetPaths.downPath, importPath);
            }

            {
                const auto filePath = "right" + std::filesystem::path(assetPaths.rightPath).extension().string();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);
                if (std::filesystem::exists(importPath)) std::filesystem::remove(importPath);
                std::filesystem::copy(assetPaths.rightPath, importPath);
            }

            {
                const auto filePath = "left" + std::filesystem::path(assetPaths.leftPath).extension().string();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);
                if (std::filesystem::exists(importPath)) std::filesystem::remove(importPath);
                std::filesystem::copy(assetPaths.leftPath, importPath);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        const auto& ncaImportPath = projectSettings.cubeMapsPath/subdirectory/(name + ".nca");
        if (std::filesystem::exists(ncaImportPath)) std::filesystem::remove(ncaImportPath);
        auto asset = CreateAsset(ncaImportPath, AssetType::Skybox);

        if(!BuildNcaFile(projectSettings.cubeMapsPath/subdirectory, AssetType::Skybox))
        {
            Output::LogError("Failure building nca file from:", (projectSettings.cubeMapsPath/subdirectory).string());
            return false;
        }

        if(!LoadAsset(asset, AssetType::Skybox))
        {
            Output::LogError("Failure loading asset:", ncaImportPath.string());
            return false;
        }

        collection.Add(std::move(asset));
        Output::Log("Added asset: " + ncaImportPath.string());
        return true;
    }

     bool AssetManifest::AddSkybox(const std::string& name)
    {
        const auto& projectSettings = m_getConfig().projectSettings;
        const auto& ncaImportPath = std::filesystem::path(projectSettings.cubeMapsPath)/name;

        auto asset = CreateAsset(ncaImportPath, AssetType::Skybox);
        auto& collection = GetCollection(AssetType::Skybox);
        collection.Add(std::move(asset));

        Output::Log("Added asset: " + ncaImportPath.string());
        return true;
    }

    bool AssetManifest::Remove(const std::filesystem::path& assetPath, AssetType type)
    {
        Output::Log("Removed asset: " + assetPath.string());

        switch (type)
        {
            case AssetType::AudioClip:
            {
                auto wavPath = assetPath;
                wavPath.replace_extension(".wav");

                if (std::filesystem::exists(wavPath)) std::filesystem::remove(wavPath);
                if (std::filesystem::exists(assetPath)) std::filesystem::remove(assetPath);
                nc::UnloadAudioClipAsset(assetPath.string());
                break;
            }
            case AssetType::ConcaveCollider:
            {
                DeleteMeshFiles(assetPath);
                nc::UnloadConcaveColliderAsset(assetPath.string());
                break;
            }
            case AssetType::HullCollider:
            {
                DeleteMeshFiles(assetPath);
                nc::UnloadConvexHullAsset(assetPath.string());
                break;
            }
            case AssetType::Mesh:
            {
                DeleteMeshFiles(assetPath);
                nc::UnloadMeshAsset(assetPath.string());
                break;
            }
            case AssetType::Skybox:
            {
                try
                {
                    if (std::filesystem::exists(assetPath)) std::filesystem::remove_all(assetPath.parent_path());
                    if (std::filesystem::exists(assetPath.parent_path())) std::filesystem::remove(assetPath.parent_path());
                } catch(std::exception) {}

                nc::UnloadCubeMapAsset(assetPath.string());
                break;
            }
            case AssetType::Texture:
            {
                if (std::filesystem::exists(assetPath)) std::filesystem::remove(assetPath);
                nc::UnloadTextureAsset(assetPath.string());
                break;
            }
        }

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
        
        for(auto& asset : manifestData.skyboxes)
        {
            if(!LoadAsset(asset, AssetType::Skybox))
                Output::LogError("Failure loading Skybox:", asset.sourcePath.string());
            
            m_skyboxes.Add(std::move(asset));
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
            case AssetType::Skybox:          return m_skyboxes;
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
            case AssetType::Skybox:          return m_skyboxes;
        }

        throw std::runtime_error("AssetManifest::GetCollection - Unknown AssetType");
    }
}