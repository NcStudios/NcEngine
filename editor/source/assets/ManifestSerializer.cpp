#include "ManifestSerializer.h"
#include "AssetManifest.h"
#include "utility/Output.h"

#include <fstream>

namespace
{
    const auto AudioClipTag = std::string{"AudioClip"};
    const auto ConcaveColliderTag = std::string{"ConcaveCollider"};
    const auto HullColliderTag = std::string{"HullCollider"};
    const auto MeshTag = std::string{"Mesh"};
    const auto TextureTag = std::string{"Texture"};

    auto ReadStandardAsset(std::ifstream& file) -> nc::editor::Asset
    {
        nc::editor::Asset out;
        std::string path;
        std::getline(file, out.name, '\n');
        std::getline(file, path, '\n');
        out.sourcePath = std::filesystem::path{std::move(path)};
        return out;
    }

    auto ReadNcaAsset(std::ifstream& file) -> nc::editor::Asset
    {
        nc::editor::Asset out;
        std::getline(file, out.name, '\n');
        std::string path;
        std::getline(file, path, '\n');
        out.sourcePath = std::filesystem::path{std::move(path)};
        std::getline(file, path, '\n');
        out.ncaPath = std::filesystem::path{std::move(path)};
        return out;
    }

    void WriteAsset(std::ofstream& file, const nc::editor::Asset& asset, const std::string& typeTag)
    {
        file << typeTag << '\n' << asset.name << '\n' << asset.sourcePath.string() << '\n';
        
        if(asset.ncaPath.has_value())
        {
            file << asset.ncaPath.value().string() << '\n';
        }
    }
}

namespace nc::editor
{
    auto ReadManifest(const std::filesystem::path& manifestPath) -> ManifestData
    {
        ManifestData out;
        out.readSuccessfully = false;

        if(!std::filesystem::directory_entry{manifestPath}.exists())
        {
            Output::LogError("Manifest doesn't exist: " + manifestPath.string());
            return out;
        }

        std::ifstream file{manifestPath};

        if(!file.is_open())
        {
            Output::LogError("Failure opening manifest: " + manifestPath.string());
            return out;
        }

        std::string line;

        while(!file.eof())
        {
            Asset asset;
            std::getline(file, line, '\n');

            if(line == AudioClipTag)
            {
                out.audioClips.push_back(ReadStandardAsset(file));
            }
            else if(line == ConcaveColliderTag)
            {
                out.concaveColliders.push_back(ReadNcaAsset(file));
            }
            else if(line == HullColliderTag)
            {
                out.hullColliders.push_back(ReadNcaAsset(file));
            }
            else if(line == MeshTag)
            {
                out.meshes.push_back(ReadNcaAsset(file));
            }
            else if(line == TextureTag)
            {
                out.textures.push_back(ReadStandardAsset(file));
            }
        }

        out.readSuccessfully = true;
        return out;
    }

    void WriteManifest(const std::filesystem::path& manifestPath, const AssetManifest& manifest)
    {
        /** @todo last update times */

        std::ofstream file{manifestPath};
        if(!file.is_open())
        {
            Output::Log("Failure opening manifest: " + manifestPath.string());
            return;
        }

        for(const auto& asset : manifest.View(AssetType::AudioClip))
            WriteAsset(file, asset, AudioClipTag);
        
        for(const auto& asset : manifest.View(AssetType::ConcaveCollider))
            WriteAsset(file, asset, ConcaveColliderTag);
        
        for(const auto& asset : manifest.View(AssetType::HullCollider))
            WriteAsset(file, asset, HullColliderTag);
        
        for(const auto& asset : manifest.View(AssetType::Mesh))
            WriteAsset(file, asset, MeshTag);

        for(const auto& asset : manifest.View(AssetType::Texture))
            WriteAsset(file, asset, TextureTag);
    }
}