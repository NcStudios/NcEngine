#include "AssetManifest.h"
#include "ManifestSerializer.h"
#include "utility/DefaultComponents.h"
#include "utility/Output.h"

#include <iostream>

namespace
{
bool IsDefaultAsset(const std::filesystem::path& path, nc::AssetType type)
{
    switch(type)
    {
        case nc::AssetType::AudioClip:
        {
            /** @todo */
            return false;
        }
        case nc::AssetType::ConcaveCollider:
        {
            return path.string() == nc::editor::PlaneConcaveColliderPath;
        }
        case nc::AssetType::HullCollider:
        {
            return path.string() == nc::editor::CubeHullColliderPath;
        }
        case nc::AssetType::Mesh:
        {
            auto str = path.string();
            return str == nc::editor::CubeMeshPath ||
                    str == nc::editor::CapsuleMeshPath ||
                    str == nc::editor::SphereMeshPath ||
                    str == nc::editor::PlaneMeshPath;
        }
        case nc::AssetType::Texture:
        {
            auto str = path.string();
            return str == nc::editor::DefaultBaseColorPath ||
                    str == nc::editor::DefaultNormalPath ||
                    str == nc::editor::DefaultRoughnessPath;
        }
        case nc::AssetType::Skybox:
        {
            return path.string() == nc::editor::DefaultSkyboxPath;
        }
        case nc::AssetType::Shader:
        {
            /** @todo */
            return false;
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

bool ReplaceSkyboxFile(const std::filesystem::path& oldFile, const std::filesystem::path& newFile)
{
    if (oldFile != newFile)
    {
        if(!std::filesystem::exists(newFile))
        {
            return false;
        }

        try 
        {
            if (std::filesystem::exists(oldFile)) std::filesystem::remove(oldFile);
            std::filesystem::copy(newFile, oldFile);
        }
        catch(const std::exception& e)
        {
            nc::editor::Output::LogError("Error replacing skybox file: ", e.what());
            return false;
        }
    }

    return true;
}
} // anonymous namespace

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
    const auto& assetSettings = m_getConfig().assetSettings;
    switch(type)
    {
        case AssetType::AudioClip:       return assetSettings.audioClipsPath / assetPath.filename();
        case AssetType::ConcaveCollider: return assetSettings.concaveCollidersPath / assetPath.filename();
        case AssetType::HullCollider:    return assetSettings.hullCollidersPath / assetPath.filename();
        case AssetType::Mesh:            return assetSettings.meshesPath / assetPath.filename();
        case AssetType::Texture:         return assetSettings.texturesPath / assetPath.filename();
        case AssetType::Skybox:          return assetSettings.cubeMapsPath / assetPath.filename();
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

bool AssetManifest::EditSkybox(const CubeMapFaces& previousPaths, const CubeMapFaces& newPaths, const std::string& name)
{
    ReplaceSkyboxFile(previousPaths.frontPath, newPaths.frontPath);
    ReplaceSkyboxFile(previousPaths.backPath,  newPaths.backPath);
    ReplaceSkyboxFile(previousPaths.upPath,    newPaths.upPath);
    ReplaceSkyboxFile(previousPaths.downPath,  newPaths.downPath);
    ReplaceSkyboxFile(previousPaths.rightPath, newPaths.rightPath);
    ReplaceSkyboxFile(previousPaths.leftPath, newPaths.leftPath);

    const auto subdirectory = std::filesystem::path(name);
    const auto& assetSettings = m_getConfig().assetSettings;
    const auto& ncaImportPath = assetSettings.cubeMapsPath/subdirectory/(name + ".nca");
    UnloadCubeMapAsset(ncaImportPath.string());

    auto asset = CreateAsset(ncaImportPath, AssetType::Skybox);
    if(!LoadAsset(asset, AssetType::Skybox))
    {
        Output::LogError("Failure loading asset:", ncaImportPath.string());
        return false;
    }

    Output::Log("Added asset: " + ncaImportPath.string());
    return true;
}

bool AssetManifest::AddSkybox(const CubeMapFaces& assetPaths, const std::string& name)
{
    auto checkForExistence = [](const std::string& path) -> bool
    {
        if(!std::filesystem::exists(path))
        {
            Output::LogError("Face path does not exist: ", path);
            return false;
        }
        return true;
    };

    if (!checkForExistence(assetPaths.frontPath)) return false;
    if (!checkForExistence(assetPaths.backPath)) return false;
    if (!checkForExistence(assetPaths.upPath)) return false;
    if (!checkForExistence(assetPaths.downPath)) return false;
    if (!checkForExistence(assetPaths.rightPath)) return false;
    if (!checkForExistence(assetPaths.leftPath)) return false;
    
    auto& collection = GetCollection(AssetType::Skybox);

    if(collection.Contains(name))
    {
        Output::LogError("Asset is already in the manifest");
        return false;
    }

    const auto subdirectory = std::filesystem::path(name);
    const auto& assetSettings = m_getConfig().assetSettings;

    if (!std::filesystem::exists(assetSettings.cubeMapsPath/subdirectory))
    {
        std::filesystem::create_directory(assetSettings.cubeMapsPath/subdirectory);
    }

    const auto importSubdirectory = std::filesystem::path{assetSettings.cubeMapsPath} / subdirectory;
    auto copyFacePath = [&importSubdirectory](const std::string& inPath, const std::string& outName)
    {
        const auto sourcePath = std::filesystem::path{inPath};
        const auto extension = sourcePath.extension().string();
        const auto filePath = std::filesystem::path{outName + extension};
        const auto importPath = importSubdirectory / filePath;
        if (std::filesystem::exists(importPath)) std::filesystem::remove(importPath);
        std::filesystem::copy(sourcePath, importPath);
    };

    try
    {
        copyFacePath(assetPaths.frontPath, "front");
        copyFacePath(assetPaths.backPath, "back");
        copyFacePath(assetPaths.upPath, "up");
        copyFacePath(assetPaths.downPath, "down");
        copyFacePath(assetPaths.rightPath, "right");
        copyFacePath(assetPaths.leftPath, "left");
    }
    catch(const std::exception& e)
    {
        Output::LogError("Error copying face path.", e.what());
    }

    const auto& ncaImportPath = assetSettings.cubeMapsPath/subdirectory/(name + ".nca");
    if (std::filesystem::exists(ncaImportPath)) std::filesystem::remove(ncaImportPath);
    auto asset = CreateAsset(ncaImportPath, AssetType::Skybox);

    if(!BuildNcaFile(assetSettings.cubeMapsPath/subdirectory, AssetType::Skybox))
    {
        Output::LogError("Failure building nca file from:", (assetSettings.cubeMapsPath/subdirectory).string());
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
    const auto& assetSettings = m_getConfig().assetSettings;
    const auto& ncaImportPath = std::filesystem::path(assetSettings.cubeMapsPath)/name;

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
            if (std::filesystem::exists(assetPath)) std::filesystem::remove_all(assetPath.parent_path());
            if (std::filesystem::exists(assetPath.parent_path())) std::filesystem::remove(assetPath.parent_path());

            nc::UnloadCubeMapAsset(assetPath.string());
            break;
        }
        case AssetType::Texture:
        {
            if (std::filesystem::exists(assetPath)) std::filesystem::remove(assetPath);
            nc::UnloadTextureAsset(assetPath.string());
            break;
        }
        case AssetType::Shader:
        {
            /** @todo */
            return false;
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
        case AssetType::Shader:          throw std::runtime_error("Not implemented"); /** @todo */
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
        case AssetType::Shader:          throw std::runtime_error("AssetManifest::GetCollection - Not implemented"); /** @todo */
    }

    throw std::runtime_error("AssetManifest::GetCollection - Unknown AssetType");
}
} // namespace nc::editor