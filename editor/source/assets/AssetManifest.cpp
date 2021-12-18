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
                const auto filePath = std::filesystem::path(assetPaths.frontPath).filename();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);

                std::cout << projectSettings.cubeMapsPath << std::endl;
                std::cout << "std::filesystem::copy(" << assetPaths.frontPath << ", " << importPath.string() <<  ")" << std::endl;
                std::filesystem::copy(assetPaths.frontPath, importPath);
            }

            {
                const auto filePath = std::filesystem::path(assetPaths.backPath).filename();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);
                std::cout << "std::filesystem::copy(" << assetPaths.backPath << ", " << importPath.string() <<  ")" << std::endl;
                std::filesystem::copy(assetPaths.backPath, importPath);
            }

            {
                const auto filePath = std::filesystem::path(assetPaths.upPath).filename();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);
                std::cout << "std::filesystem::copy(" << assetPaths.upPath << ", " << importPath.string() <<  ")" << std::endl;
                std::filesystem::copy(assetPaths.upPath, importPath);
            }

            {
                const auto filePath = std::filesystem::path(assetPaths.downPath).filename();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);
                std::cout << "std::filesystem::copy(" << assetPaths.downPath << ", " << importPath.string() <<  ")" << std::endl;
                std::filesystem::copy(assetPaths.downPath, importPath);
            }

            {
                const auto filePath = std::filesystem::path(assetPaths.rightPath).filename();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);
                std::cout << "std::filesystem::copy(" << assetPaths.rightPath << ", " << importPath.string() <<  ")" << std::endl;
                std::filesystem::copy(assetPaths.rightPath, importPath);
            }

            {
                const auto filePath = std::filesystem::path(assetPaths.leftPath).filename();
                const auto importPath = std::filesystem::path(projectSettings.cubeMapsPath/subdirectory/filePath);  // importPath = "nc/project/textures/cubemaps/Test/front.jpg"
                std::cout << "std::filesystem::copy(" << assetPaths.leftPath << ", " << importPath.string() <<  ")" << std::endl;
                std::filesystem::copy(assetPaths.leftPath, importPath);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        std::cout << " Value of subdirectory: " << subdirectory << std::endl;

        const auto& ncaImportPath = projectSettings.cubeMapsPath/subdirectory/(name + ".nca");
        auto asset = CreateAsset(ncaImportPath, AssetType::Skybox);

        if(!BuildNcaFile(projectSettings.cubeMapsPath/subdirectory, AssetType::Skybox))
        {
            Output::LogError("Failure building nca file from:", (projectSettings.cubeMapsPath/subdirectory).string());
            return false;
        }

        std::cout << " Value of asset soruce path: " << asset.sourcePath << std::endl;

        if(!LoadAsset(asset, AssetType::Skybox))
        {
            Output::LogError("Failure loading asset:", ncaImportPath.string());
            return false;
        }

        collection.Add(std::move(asset));
        Output::Log("Added asset: " + ncaImportPath.string());
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