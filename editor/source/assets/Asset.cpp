#include "Asset.h"
#include "utility/Output.h"

// Engine includes
#include "Assets.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "platform/win32/NcWin32.h"

#include <iostream>

namespace nc::editor
{
    const auto FbxExtension = std::string{".fbx"};
    const auto WaveExtension = std::string{".wav"};
    const auto PngExtension = std::string{".png"};
    const auto ExeName = std::string{"tools/asset_builder/build.exe"};

    std::string BuildAssetTypeArg(nc::editor::AssetType type)
    {
        if(type == nc::editor::AssetType::Mesh)
            return std::string{" -a mesh "};
        if(type == nc::editor::AssetType::ConcaveCollider)
            return std::string{" -a concave-collider "};
        if(type == nc::editor::AssetType::HullCollider)
            return std::string{" -a hull-collider "};

        return std::string{};
    }

    Asset CreateAsset(const std::filesystem::path& path, AssetType type)
    {
        Asset out
        {
            .name = path.stem().string(),
            .sourcePath = path,
            .ncaPath = std::optional<std::filesystem::path>{}
        };

        if(RequiresNcaFile(type))
        {
            auto ncaPath = path;
            ncaPath.replace_extension(".nca");
            out.ncaPath = std::optional<std::filesystem::path>{ncaPath.string()};
        }

        return out;
    }

    bool BuildNcaFile(const std::filesystem::path& path, AssetType type)
    {
        STARTUPINFOA startupInfo;
        PROCESS_INFORMATION processInfo;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        ZeroMemory(&processInfo, sizeof(processInfo));

        auto outPath = path;
        outPath.remove_filename();

        std::string cmdLine = ExeName + BuildAssetTypeArg(type) + std::string{" -t "} + path.string() + std::string{" -o "} + outPath.string();

        auto result = CreateProcessA(TEXT(ExeName.data()), TEXT(cmdLine.data()), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);

        if(!result)
        {
            std::cerr << "BuildNcaFile - error: " << result << '\n';
            return false;
        }

        WaitForSingleObject(processInfo.hProcess, INFINITE);
        DWORD exitCode;
        result = GetExitCodeProcess(processInfo.hProcess, &exitCode);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        if(!result)
        {
            std::cerr << "BuildNcaFile - error: couldn't get exit code\n";
            return false;
        }
        
        return true;
    }

    bool LoadAsset(const Asset& asset, AssetType type)
    {
        /** @todo In the case of an exception, we need to load default asset */

        try
        {
            switch(type)
            {
                case AssetType::AudioClip:       { nc::LoadSoundClipAsset(asset.sourcePath); break; }
                case AssetType::ConcaveCollider: { nc::LoadConcaveColliderAsset(asset.ncaPath.value()); break; }
                case AssetType::HullCollider:    { nc::LoadConvexHullAsset(asset.ncaPath.value()); break; }
                case AssetType::Mesh:            { /** @todo add once fixed*/ break; }
                case AssetType::Texture:         { /** @todo add once fixed*/ break; }
                default:                         { return false; }
            }

            return true;
        }
        catch(const std::exception& e)
        {
            Output::Log("LoadAsset - exception: " + std::string{e.what()});
        }

        return false;
    }

    bool HasValidExtensionForAssetType(const std::filesystem::path& assetPath, AssetType type)
    {
        /** @todo We do support other extensions (obj, jpeg, etc). Need to test to make sure
         *  they work before adding them here. */
        auto extension = assetPath.extension().string();

        switch(type)
        {
            case AssetType::AudioClip:       return extension == WaveExtension;
            case AssetType::ConcaveCollider: return extension == FbxExtension;
            case AssetType::HullCollider:    return extension == FbxExtension;
            case AssetType::Mesh:            return extension == FbxExtension;
            case AssetType::Texture:         return extension == PngExtension;
        }

        return false;
    }

    bool RequiresNcaFile(AssetType type)
    {
        return type == AssetType::ConcaveCollider || type == AssetType::HullCollider || type == AssetType::Mesh;
    }
}