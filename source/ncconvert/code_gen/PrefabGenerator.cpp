#include "PrefabGenerator.h"

#include "utility/EnumExtensions.h"

#include "../builder/Target.h"
#include "builder/Manifest.h"

#include <array>
#include <fstream>
#include <ranges>

namespace
{
void BeginNamespace(std::ostream& stream, std::string_view ns)
{
    stream << "namespace " << ns << "\n{\n";
}

void EndNamespace(std::ostream& stream, std::string_view ns)
{
    stream << "} // namespace " << ns << "\n\n";
}

void BeginHeader(std::ostream& header, std::string_view ns)
{
    header << "/** @note This file is auto generated. */\n\n"
           << "#pragma once\n\n"
           << "#include \"ncengine/asset/AssetViews.h\"\n\n";

    BeginNamespace(header, ns);
}

void EndHeader(std::ostream& header, std::string_view ns)
{
    EndNamespace(header, ns);
}

void BeginSource(std::ostream& source, std::string_view ns)
{
    source << "/** @note This file is auto generated. */\n\n"
           << "#include \"GeneratedAssets.h\"\n\n"
           << "#include \"ncengine/asset/Assets.h\"\n"
           << "#include \"ncutility/Hash.h\"\n\n"
           << "#include <array>\n"
           << "#include <string>\n\n";

    BeginNamespace(source, ns);
}

void EndSource(std::ostream& source, std::string_view ns)
{
    EndNamespace(source, ns);
}

void WritePaths(std::ostream& header,
                const std::vector<nc::convert::ReflectedTarget>& assets)
{
    BeginNamespace(header, "path");
    for (const auto& asset : assets)
    {
        header << "constexpr auto " << asset.name << " = \"" << asset.path << "\";\n";
    }

    EndNamespace(header, "path");
}

void WriteVariableDeclarations(std::ostream& header,
                               const std::vector<nc::convert::ReflectedTarget>& assets,
                               std::string_view type)
{
    for (const auto& asset : assets)
    {
        header << "extern " << type << ' ' << asset.name << ";\n";
    }

    header << '\n';
}

void WriteVariableDefinitions(std::ostream& source,
                              const std::vector<nc::convert::ReflectedTarget>& assets,
                              std::string_view type)
{
    for (const auto& asset : assets)
    {
        source << type << ' ' << asset.name << "{};\n";
    }

    source << '\n';
}

void WritePathsArray(std::ostream& source,
                     const std::vector<nc::convert::ReflectedTarget>& assets,
                     std::string_view variableName = "g_paths")
{
    source << "const auto " << variableName << " = std::array{\n";
    for (const auto& asset : assets)
    {
        source << "    std::string{path::" << asset.name << "},\n";
    }

    source << "};\n\n";
}

void WriteAssetIds(std::ostream& source,
                   const std::vector<nc::convert::ReflectedTarget>& assets)
{
    for (const auto& asset : assets)
    {
        source << "nc::asset::AssetId " << asset.name << "{nc::utility::Fnv1a(path::" << asset.name << ")};\n";
    }

    source << '\n';
}

void WriteLoadFunctionDeclaration(std::ostream& header)
{
    header << "void Load();\n\n";
}

void WriteLoadFunction(std::ostream& source,
                       std::string_view functionName)
{
    source << "void Load()\n{\n    " << functionName << "(g_paths);\n}\n\n";
}

void WriteAcquireFunctionDeclaration(std::ostream& header)
{
    header << "void Acquire();\n";
}

void WriteAcquireFunction(std::ostream& source,
                          const std::vector<nc::convert::ReflectedTarget>& assets,
                          std::string_view functionName)
{
    source << "void Acquire()\n{\n";
    for (const auto& asset : assets)
    {
        source << "    " << asset.name << " = " << functionName << "(path::" << asset.name << ");\n";
    }

    source << "}\n\n";
}

void WriteAnimations(std::ostream& header,
                     std::ostream& source,
                     const std::vector<nc::convert::ReflectedTarget>& assets)
{
    constexpr auto ns = std::string_view{"animation"};
    constexpr auto variableType = std::string_view{"nc::asset::AssetId"};
    BeginNamespace(header, ns);
    WritePaths(header, assets);
    WriteVariableDeclarations(header, assets, variableType);
    WriteLoadFunctionDeclaration(header);
    EndNamespace(header, ns);

    BeginNamespace(source, ns);
    WritePathsArray(source, assets);
    WriteAssetIds(source, assets);
    WriteLoadFunction(source, "nc::asset::LoadSkeletalAnimationAssets");
    EndNamespace(source, ns);
}

void WriteAudioClips(std::ostream& header,
                     std::ostream& source,
                     const std::vector<nc::convert::ReflectedTarget>& assets)
{
    constexpr auto ns = std::string_view{"audio_clip"};
    BeginNamespace(header, ns);
    WritePaths(header, assets);
    WriteLoadFunctionDeclaration(header);
    EndNamespace(header, ns);

    BeginNamespace(source, ns);
    WritePathsArray(source, assets);
    WriteLoadFunction(source, "nc::asset::LoadAudioClipAssets");
    EndNamespace(source, ns);
}

void WriteConvexHulls(std::ostream& header,
                      std::ostream& source,
                      const std::vector<nc::convert::ReflectedTarget>& assets)
{
    constexpr auto ns = std::string_view{"convex_hull"};
    BeginNamespace(header, ns);
    WritePaths(header, assets);
    WriteVariableDeclarations(header, assets, "nc::asset::AssetId");
    WriteLoadFunctionDeclaration(header);
    EndNamespace(header, ns);

    BeginNamespace(source, ns);
    WritePathsArray(source, assets);
    WriteAssetIds(source, assets);
    WriteLoadFunction(source, "nc::asset::LoadConvexHullAssets");
    EndNamespace(source, ns);
}

void WriteCubeMaps(std::ostream& header,
                   std::ostream& source,
                   const std::vector<nc::convert::ReflectedTarget>& assets)
{
    constexpr auto ns = std::string_view{"cube_map"};
    BeginNamespace(header, ns);
    WritePaths(header, assets);
    WriteLoadFunctionDeclaration(header);
    EndNamespace(header, ns);

    BeginNamespace(source, ns);
    WritePathsArray(source, assets);
    WriteLoadFunction(source, "nc::asset::LoadCubeMapAssets");
    EndNamespace(source, ns);
}

void WriteMeshes(std::ostream& header,
                 std::ostream& source,
                 const std::vector<nc::convert::ReflectedTarget>& assets)
{
    constexpr auto ns = std::string_view{"mesh"};
    constexpr auto varType = std::string_view{"nc::asset::MeshView"};
    BeginNamespace(header, ns);
    WritePaths(header, assets);
    WriteVariableDeclarations(header, assets, varType);
    WriteLoadFunctionDeclaration(header);
    WriteAcquireFunctionDeclaration(header);
    EndNamespace(header, ns);

    BeginNamespace(source, ns);
    WritePathsArray(source, assets);
    WriteVariableDefinitions(source, assets, varType);
    WriteLoadFunction(source, "nc::asset::LoadMeshAssets");
    WriteAcquireFunction(source, assets, "nc::asset::AcquireMeshAsset");
    EndNamespace(source, ns);
}

void WriteMeshColliders(std::ostream& header,
                        std::ostream& source,
                        const std::vector<nc::convert::ReflectedTarget>& assets)
{
    constexpr auto ns = std::string_view{"mesh_collider"};
    BeginNamespace(header, ns);
    WritePaths(header, assets);
    WriteVariableDeclarations(header, assets, "nc::asset::AssetId");
    WriteLoadFunctionDeclaration(header);
    EndNamespace(header, ns);

    BeginNamespace(source, ns);
    WritePathsArray(source, assets);
    WriteAssetIds(source, assets);
    WriteLoadFunction(source, "nc::asset::LoadMeshColliderAssets");
    EndNamespace(source, ns);
}

void WriteTextures(std::ostream& header,
                   std::ostream& source,
                   const std::vector<nc::convert::ReflectedTarget>& assets)
{
    constexpr auto ns = std::string_view{"texture"};
    BeginNamespace(header, ns);
    WritePaths(header, assets);
    WriteVariableDeclarations(header, assets, "nc::asset::TextureView");
    WriteLoadFunctionDeclaration(header);
    WriteAcquireFunctionDeclaration(header);
    EndNamespace(header, ns);

    BeginNamespace(source, ns);

    auto diffuse = std::vector<nc::convert::ReflectedTarget>{};
    auto normal = std::vector<nc::convert::ReflectedTarget>{};
    for (const auto& asset : assets)
    {
        if (asset.subtype == nc::convert::AssetSubtype::NormalTexture)
        {
            normal.push_back(asset);
        }
        else
        {
            diffuse.push_back(asset);
        }
    }

    WritePathsArray(source, diffuse, "g_diffusePaths");
    WritePathsArray(source, normal, "g_normalPaths");
    WriteVariableDefinitions(source, assets, "nc::asset::TextureView");

    source << "void Load()\n"
            << "{\n"
            << "    nc::asset::LoadTextureAssets(g_diffusePaths, false, nc::asset::AssetFlags::TextureTypeImage);\n"
            << "    nc::asset::LoadTextureAssets(g_normalPaths, false, nc::asset::AssetFlags::TextureTypeNormalMap);\n"
            << "}\n";

    WriteAcquireFunction(source, assets, "nc::asset::AcquireTextureAsset");
    EndNamespace(source, ns);
}
} // anonymous namespace

namespace nc::convert
{
void GeneratePrefabFile(const std::filesystem::path& manifestPath,
                        const std::filesystem::path& outputDirectory,
                        const std::string& rootNamespace)
{
    auto manifest = Manifest{manifestPath};
    auto assetList = manifest.GetTargetsForSourceGeneration();

    if (!std::filesystem::exists(outputDirectory))
    {
        std::filesystem::create_directories(outputDirectory);
    }

    auto header = std::ofstream{outputDirectory / "GeneratedAssets.h"};
    auto source = std::ofstream{outputDirectory / "GeneratedAssets.cpp"};

    if (!header.is_open() || !source.is_open())
    {
        throw "failed to create files";
    }

    BeginHeader(header, rootNamespace);
    BeginSource(source, rootNamespace);

    for (const auto& [type, assets] : assetList)
    {
        using enum nc::asset::AssetType;
        switch (type)
        {
            case AudioClip:         WriteAudioClips(header, source, assets);    break;
            case ConvexHull:        WriteConvexHulls(header, source, assets);   break;
            case CubeMap:           WriteCubeMaps(header, source, assets);      break;
            case Mesh:              WriteMeshes(header, source, assets);        break;
            case MeshCollider:      WriteMeshColliders(header, source, assets); break;
            case SkeletalAnimation: WriteAnimations(header, source, assets);    break;
            case Texture:           WriteTextures(header, source, assets);      break;
            case Shader:
            case Font:
                continue;
        }
    }

    EndHeader(header, rootNamespace);
    EndSource(source, rootNamespace);
}
} // namespace nc::convert
