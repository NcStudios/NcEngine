#include "LoaderScene.h"

#include <algorithm>
#include <fstream>

namespace
{
    void WriteLoadsForType(std::ofstream& file, const nc::editor::AssetManifest& manifest, nc::editor::AssetType type, const char* funcName)
    {
        static constexpr const char* tab = "    ";
        static constexpr const char* pathListType = "std::vector<std::string>";

        auto assets = manifest.View(type);
        if(assets.size() == 0)
            return;

        file << tab << funcName << '(' << pathListType << '{' << '\n';

        for(const auto& asset : assets)
        {
            file << tab << asset.sourcePath << ",\n";
        }

        file << tab << "}, true);\n";
    }
}

namespace nc::editor
{
    void WriteLoaderScene(const std::filesystem::path& projectDirectory, const AssetManifest& manifest)
    {
        std::ofstream file{projectDirectory / "scenes\\GeneratedLoadScene.h"};
        file << "#pragma once\n\n"
             << "#include \"assets\\AssetUtilities.h\"\n"
             << "#include \"Scene.h\"\n\n"
             << "#include <string>\n"
             << "#include <vector>\n\n"
             << "namespace nc::editor\n"
             << "{\n"
             << "class GeneratedLoadScene : public Scene\n"
             << "{\n"
             << "public:\n"
             << "void Load(NcEngine*) override\n"
             << "{\n";

        WriteLoadsForType(file, manifest, AssetType::AudioClip,       "LoadAudioClipAssets");
        WriteLoadsForType(file, manifest, AssetType::ConcaveCollider, "LoadConcaveColliderAssets");
        WriteLoadsForType(file, manifest, AssetType::HullCollider,    "LoadConvexHullAssets");
        WriteLoadsForType(file, manifest, AssetType::Mesh,            "LoadMeshAssets");
        WriteLoadsForType(file, manifest, AssetType::Texture,         "LoadTextureAssets");
        WriteLoadsForType(file, manifest, AssetType::Skybox,          "LoadCubeMapAssets");

        file << "}\n\n"
             << "void Unload() override\n{\n}\n"
             << "};\n}\n";
    }
}