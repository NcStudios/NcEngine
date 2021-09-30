#include "AssetManifest.h"
#include "BuildNca.h"
#include "utility/Output.h"

// From Engine
#include "Assets.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"

#include <algorithm>
#include <fstream>


#include <iostream>

namespace
{
    bool Contains(const std::filesystem::path& path, const std::vector<nc::editor::Asset>& assets)
    {
        auto pos = std::ranges::find_if(assets, [&path](const auto& asset)
        {
            return asset.sourcePath == path;
        });

        return pos != assets.end();
    }

    bool AddNcaAsset(const std::filesystem::path& path, std::vector<nc::editor::Asset>& assets, nc::editor::AssetType type)
    {
        auto entry = std::filesystem::directory_entry{path};
        if(!entry.exists())
        {
            nc::editor::Output::Log("AddNcaAsset - Bad path: " + path.string());
            return false;
        }

        auto ncaPath = path;
        ncaPath.replace_extension(".nca");
        assets.emplace_back
        (
            path.stem().string(),
            path,
            std::optional<std::filesystem::path>{ncaPath.string()},
            entry.last_write_time()
        );
        
        if(nc::editor::BuildNcaFile(path, type))
            return true;

        nc::editor::Output::Log("AddNcaAsset - Failure building nca file for: " + path.string());
        return false;
    }

    bool AddAsset(const std::filesystem::path& path, std::vector<nc::editor::Asset>& assets)
    {
        auto entry = std::filesystem::directory_entry{path};
        if(!entry.exists())
        {
            nc::editor::Output::Log("AddAsset - Bad path: " + path.string());
            return false;
        }

        assets.emplace_back
        (
            path.stem().string(),
            path,
            std::optional<std::filesystem::path>{},
            entry.last_write_time()
        );

        return true;
    }
}

namespace nc::editor
{
    bool AssetManifest::Add(const std::filesystem::path& assetPath, AssetType type)
    {
        switch(type)
        {
            case AssetType::AudioClip:       return Add<AssetType::AudioClip>(assetPath);
            case AssetType::ConcaveCollider: return Add<AssetType::ConcaveCollider>(assetPath);
            case AssetType::HullCollider:    return Add<AssetType::HullCollider>(assetPath);
            case AssetType::Mesh:            return Add<AssetType::Mesh>(assetPath);
            case AssetType::Texture:         return Add<AssetType::Texture>(assetPath);
        }

        return false;
    }

    template<> bool AssetManifest::Add<AssetType::AudioClip>(const std::filesystem::path& assetPath)
    {
        if(Contains<AssetType::AudioClip>(assetPath))
        {
            Output::Log("AssetManifest::Add<AudioClip> - Manifest already has asset: " + assetPath.string());
            return false;
        }

        if(AddAsset(assetPath, m_audioClips))
        {
            try
            {
                LoadSoundClipAsset(assetPath);
                return true;
            }
            catch(const std::runtime_error& e)
            {
                Output::Log("AssetManifest::Add<AudioClip> - exception: " + std::string{e.what()});
            }
        }

        return false;
    }

    template<> bool AssetManifest::Add<AssetType::ConcaveCollider>(const std::filesystem::path& assetPath)
    {
        if(Contains<AssetType::ConcaveCollider>(assetPath))
        {
            Output::Log("AssetManifest::Add<ConcaveCollider> - Manifest already has asset: " + assetPath.string());
            return false;
        }

        if(AddNcaAsset(assetPath, m_concaveColliders, AssetType::ConcaveCollider))
        {
            try
            {
                LoadConcaveColliderAsset(assetPath);
                return true;
            }
            catch(const std::exception& e)
            {
                Output::Log("AssetManifest::Add<ConcaveCollider> - exception: " + std::string{e.what()});
            }
        }

        return false;
    }

    template<> bool AssetManifest::Add<AssetType::HullCollider>(const std::filesystem::path& assetPath)
    {
        if(Contains<AssetType::HullCollider>(assetPath))
        {
            Output::Log("AssetManifest::Add<HullCollider> - Manifest already has asset: " + assetPath.string());
            return false;
        }

        if(AddNcaAsset(assetPath, m_hullColliders, AssetType::HullCollider))
        {
            try
            {
                LoadConvexHullAsset(assetPath);
                return true;
            }
            catch(const std::exception& e)
            {
                Output::Log("AssetManifest::Add<HullCollider> - exception: " + std::string{e.what()});
            }
        }

        return false;
    }

    template<> bool AssetManifest::Add<AssetType::Mesh>(const std::filesystem::path& assetPath)
    {
        if(Contains<AssetType::Mesh>(assetPath))
        {
            Output::Log("AssetManifest::Add<Mesh> - Manifest already has asset: " + assetPath.string());
            return false;
        }
        
        if(AddNcaAsset(assetPath, m_meshes, AssetType::Mesh))
        {
            return true;

            /** @todo add once fixed
            try
            {
                std::cerr << "loading\n";
                graphics::LoadMeshes(std::vector<std::string>{assetPath.string()});
                std::cerr << "done loading\n";
                return true;
            }
            catch(const std::exception& e)
            {
                Output::Log("AssetManifest::Add<Mesh> - exception: " + std::string{e.what()});
            }
            */
        }

        return false;
    }

    template<> bool AssetManifest::Add<AssetType::Texture>(const std::filesystem::path& assetPath)
    {
        if(Contains<AssetType::Texture>(assetPath))
        {
            Output::Log("AssetManifest::Add<Texture> - Manifest already has asset: " + assetPath.string());
            return false;
        }

        std::cerr << "doesn't contain\n";

        if(AddAsset(assetPath, m_textures))
        {
            return true;
            
            /** @todo add once fixed
            try
            {
                std::cerr << "adding\n";
                graphics::LoadTextures(std::vector<std::string>{assetPath.string()});
                std::cerr << "dont adding\n";
                return true;
            }
            catch(const std::exception& e)
            {
                Output::Log("AssetManifest::Add<Texture> - exception: " + std::string{e.what()});
            }
            */
        }

        return false;
    }

    void AssetManifest::Remove(const std::filesystem::path& assetPath, AssetType type)
    {
        switch(type)
        {
            case AssetType::AudioClip:       return Remove<AssetType::AudioClip>(assetPath);
            case AssetType::ConcaveCollider: return Remove<AssetType::ConcaveCollider>(assetPath);
            case AssetType::HullCollider:    return Remove<AssetType::HullCollider>(assetPath);
            case AssetType::Mesh:            return Remove<AssetType::Mesh>(assetPath);
            case AssetType::Texture:         return Remove<AssetType::Texture>(assetPath);
        }
    }

    template<> void AssetManifest::Remove<AssetType::AudioClip>(const std::filesystem::path& assetPath)
    {
        auto pos = std::ranges::find_if(m_audioClips, [&assetPath](const auto& asset)
        {
            return asset.sourcePath == assetPath;
        });

        if(pos == m_audioClips.end())
        {
            Output::Log("AssetManifest::Remove - Asset does not exist: " + assetPath.string());
            return;
        }

        /** @todo unload once implemented */

        *pos = std::move(m_audioClips.back());
        m_audioClips.pop_back();
    }

    template<> void AssetManifest::Remove<AssetType::ConcaveCollider>(const std::filesystem::path& assetPath)
    {
        auto pos = std::ranges::find_if(m_concaveColliders, [&assetPath](const auto& asset)
        {
            return asset.sourcePath == assetPath;
        });

        if(pos == m_concaveColliders.end())
        {
            Output::Log("AssetManifest::Remove - Asset does not exist: " + assetPath.string());
            return;
        }

        /** @todo unload once implemented */

        *pos = std::move(m_concaveColliders.back());
        m_concaveColliders.pop_back();
    }

    template<> void AssetManifest::Remove<AssetType::HullCollider>(const std::filesystem::path& assetPath)
    {
        auto pos = std::ranges::find_if(m_hullColliders, [&assetPath](const auto& asset)
        {
            return asset.sourcePath == assetPath;
        });

        if(pos == m_hullColliders.end())
        {
            Output::Log("AssetManifest::Remove - Asset does not exist: " + assetPath.string());
            return;
        }

        /** @todo unload once implemented */

        *pos = std::move(m_hullColliders.back());
        m_hullColliders.pop_back();
    }

    template<> void AssetManifest::Remove<AssetType::Mesh>(const std::filesystem::path& assetPath)
    {
        auto pos = std::ranges::find_if(m_meshes, [&assetPath](const auto& asset)
        {
            return asset.sourcePath == assetPath;
        });

        if(pos == m_meshes.end())
        {
            Output::Log("AssetManifest::Remove - Asset does not exist: " + assetPath.string());
            return;
        }

        /** @todo unload once implemented */

        *pos = std::move(m_meshes.back());
        m_meshes.pop_back();
    }

    template<> void AssetManifest::Remove<AssetType::Texture>(const std::filesystem::path& assetPath)
    {
        auto pos = std::ranges::find_if(m_textures, [&assetPath](const auto& asset)
        {
            return asset.sourcePath == assetPath;
        });

        if(pos == m_textures.end())
        {
            Output::Log("AssetManifest::Remove - Asset does not exist: " + assetPath.string());
            return;
        }

        /** @todo unload once implemented */

        *pos = std::move(m_textures.back());
        m_textures.pop_back();
    }

    bool AssetManifest::Contains(const std::filesystem::path& assetPath, AssetType type) const
    {
        switch(type)
        {
            case AssetType::AudioClip:       return Contains<AssetType::AudioClip>(assetPath);
            case AssetType::ConcaveCollider: return Contains<AssetType::ConcaveCollider>(assetPath);
            case AssetType::HullCollider:    return Contains<AssetType::HullCollider>(assetPath);
            case AssetType::Mesh:            return Contains<AssetType::Mesh>(assetPath);
            case AssetType::Texture:         return Contains<AssetType::Texture>(assetPath);
        }

        return false;
    }

    template<> bool AssetManifest::Contains<AssetType::AudioClip>(const std::filesystem::path& assetPath) const
    {
        return ::Contains(assetPath, m_audioClips);
    }

    template<> bool AssetManifest::Contains<AssetType::ConcaveCollider>(const std::filesystem::path& assetPath) const
    {
        return ::Contains(assetPath, m_concaveColliders);
    }

    template<> bool AssetManifest::Contains<AssetType::HullCollider>(const std::filesystem::path& assetPath) const
    {
        return ::Contains(assetPath, m_hullColliders);
    }

    template<> bool AssetManifest::Contains<AssetType::Mesh>(const std::filesystem::path& assetPath) const
    {
        return ::Contains(assetPath, m_meshes);
    }

    template<> bool AssetManifest::Contains<AssetType::Texture>(const std::filesystem::path& assetPath) const
    {
        return ::Contains(assetPath, m_textures);
    }

    auto AssetManifest::GetAll(AssetType type) const -> std::span<const Asset>
    {
        switch(type)
        {
            case AssetType::AudioClip:       return GetAll<AssetType::AudioClip>();
            case AssetType::ConcaveCollider: return GetAll<AssetType::ConcaveCollider>();
            case AssetType::HullCollider:    return GetAll<AssetType::HullCollider>();
            case AssetType::Mesh:            return GetAll<AssetType::Mesh>();
            case AssetType::Texture:         return GetAll<AssetType::Texture>();
        }

        return {};
    }

    template<> auto AssetManifest::GetAll<AssetType::AudioClip>() const -> std::span<const Asset>
    {
        return std::span<const Asset>{m_audioClips};
    }

    template<> auto AssetManifest::GetAll<AssetType::ConcaveCollider>() const -> std::span<const Asset>
    {
        return std::span<const Asset>{m_concaveColliders};
    }

    template<> auto AssetManifest::GetAll<AssetType::HullCollider>() const -> std::span<const Asset>
    {
        return std::span<const Asset>{m_hullColliders};
    }

    template<> auto AssetManifest::GetAll<AssetType::Mesh>() const -> std::span<const Asset>
    {
        return std::span<const Asset>{m_meshes};
    }

    template<> auto AssetManifest::GetAll<AssetType::Texture>() const -> std::span<const Asset>
    {
        return std::span<const Asset>{m_textures};
    }

    void AssetManifest::ReadManifest(const std::filesystem::path& projectDirectory)
    {
        auto manifestPath = projectDirectory / "assets/manifest.txt";
        auto manifestEntry = std::filesystem::directory_entry{manifestPath};

        if(!manifestEntry.exists())
        {
            Output::Log("AssetManifest::ReadManifest - No manifest exists");
        }
    }

    void AssetManifest::WriteManifest(const std::filesystem::path& projectDirectory) const
    {
        auto manifestPath = projectDirectory / "assets/manifest.txt";

        std::ofstream file{manifestPath};
        if(!file.is_open())
        {
            Output::Log("AssetManifest::WriteManifest - Failure opening file: " + manifestPath.string());
            return;
        }

        /** @todo last update times */

        for(const auto& asset : m_audioClips)
        {
            file << "AudioClip\n" << asset.name << '\n' << asset.sourcePath << '\n';
        }

        for(const auto& asset : m_concaveColliders)
        {
            file << "ConcaveCollider\n" << asset.name << '\n' << asset.sourcePath << '\n' << asset.ncaPath.value() << '\n';
        }

        for(const auto& asset : m_hullColliders)
        {
            file << "HullCollider\n" << asset.name << '\n' << asset.sourcePath << '\n' << asset.ncaPath.value() << '\n';
        }

        for(const auto& asset : m_meshes)
        {
            file << "Mesh\n" << asset.name << '\n' << asset.sourcePath << '\n' << asset.ncaPath.value() << '\n';
        }

        for(const auto& asset : m_textures)
        {
            file << "Texture\n" << asset.name << '\n' << asset.sourcePath << '\n';
        }
    }
}