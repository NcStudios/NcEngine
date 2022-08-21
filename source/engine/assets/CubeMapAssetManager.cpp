#include "CubeMapAssetManager.h"
#include "assets/AssetUtilities.h"
#include "stb/stb_image.h"

#include <array>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace nc
{
CubeMapAssetManager::CubeMapAssetManager(const std::string& cubeMapAssetDirectory, uint32_t maxCubeMapsCount)
    : m_cubeMapData{},
        m_assetDirectory{cubeMapAssetDirectory},
        m_maxCubeMapsCount{maxCubeMapsCount},
        m_onUpdate{}
{
    m_cubeMapData.reserve(m_maxCubeMapsCount);
}

CubeMapFaces ReadFacePathsFromAsset(std::ifstream& file, const std::filesystem::path& parentPath)
{
    CubeMapFaces facePaths{};

    std::string currentFace;
    for(size_t i = 0; i < 6; ++i)
    {
        if(file.fail())
            throw nc::NcError("Failure reading file");
        
        file >> currentFace;
        const std::filesystem::path currentFacePath = currentFace;

                if (currentFacePath.stem().string() == "front") facePaths.frontPath = (parentPath / currentFacePath).string();
        else if (currentFacePath.stem().string() == "back")  facePaths.backPath  = (parentPath / currentFacePath).string();
        else if (currentFacePath.stem().string() == "up")    facePaths.upPath    = (parentPath / currentFacePath).string();
        else if (currentFacePath.stem().string() == "down")  facePaths.downPath  = (parentPath / currentFacePath).string();
        else if (currentFacePath.stem().string() == "right") facePaths.rightPath = (parentPath / currentFacePath).string();
        else if (currentFacePath.stem().string() == "left")  facePaths.leftPath  = (parentPath / currentFacePath).string();
    }

    return facePaths;
}

/** If external (from the editor) path will be: D:\\NC\\Projects\\Test\\assets\\Textures\\Cubemaps\\Church001\\Church001.nca"
 *  If internal (called programmatically) path will be: Church001\\Church001.nca"
 * */
bool CubeMapAssetManager::Load(const std::string& path, bool isExternal)
{
    uint32_t nextCubeMapIndex = static_cast<uint32_t>(m_cubeMapData.size());

    if(IsLoaded(path))
        return false;

    std::array<stbi_uc*, 6> pixelArray = {};
    int32_t width, height, numChannels; // Same for all faces.

    if(!HasValidAssetExtension(path))
        throw nc::NcError("Invalid extension: " + path);

    std::filesystem::path inputPath = path;
    const std::filesystem::path fullyQualifiedPath = isExternal ? inputPath :  m_assetDirectory / inputPath;
    const std::filesystem::path fullyQualifiedParentDirectory = fullyQualifiedPath.parent_path();

    // Need to pass in fully qualifed path to the nca
    std::ifstream file{fullyQualifiedPath};
    if(!file.is_open())
        throw nc::NcError("Could not open file: " + fullyQualifiedPath.string());

    // Need to pass in fully qualified path minus Church001.nca
    auto faces = ReadFacePathsFromAsset(file, fullyQualifiedParentDirectory);

    /** Front face */
    pixelArray.at(0) = stbi_load(faces.frontPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
    if(!pixelArray.at(0)) throw nc::NcError("Failed to load texture file: " + faces.frontPath);

    /** Back face */
    pixelArray.at(1) = stbi_load(faces.backPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
    if(!pixelArray.at(1)) throw nc::NcError("Failed to load texture file: " + faces.backPath);

    /** Up face */
    pixelArray.at(2) = stbi_load(faces.upPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
    if(!pixelArray.at(2)) throw nc::NcError("Failed to load texture file: " + faces.upPath);

    /** Down face */
    pixelArray.at(3) = stbi_load(faces.downPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
    if(!pixelArray.at(3)) throw nc::NcError("Failed to load texture file: " + faces.downPath);

    /** Right face */
    pixelArray.at(4) = stbi_load(faces.rightPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
    if(!pixelArray.at(4)) throw nc::NcError("Failed to load texture file: " + faces.rightPath);

    /** Left face */
    pixelArray.at(5) = stbi_load(faces.leftPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
    if(!pixelArray.at(5)) throw nc::NcError("Failed to load texture file: " + faces.leftPath);

    auto cubeMapView = CubeMapView
    {
        .usage = CubeMapUsage::Skybox,
        .index = nextCubeMapIndex++
    };

    m_cubeMapData.emplace_back(pixelArray, width, height, width * height * STBI_rgb_alpha * 6, path);

    m_onUpdate.Emit
    (
        CubeMapBufferData(UpdateAction::Load, std::vector<std::string>{path}, std::span<const CubeMapData>{&m_cubeMapData.back(), 1})
    );

    return true;
}

bool CubeMapAssetManager::Load(std::span<const std::string> paths, bool isExternal)
{
    const auto newCubeMapCount = paths.size();
    const auto newCubeMapIndex = static_cast<uint32_t>(m_cubeMapData.size());
    auto itemIndex = newCubeMapIndex;

    if (newCubeMapCount + newCubeMapIndex >= m_maxCubeMapsCount)
        throw NcError("Cannot exceed max texture count.");

    auto idsToLoad = std::vector<std::string>{};
    idsToLoad.reserve(paths.size());

    for (const auto& path : paths)
    {
        if(IsLoaded(path)) continue;

        if(!HasValidAssetExtension(path))
        throw nc::NcError("Invalid extension: " + path);

        std::filesystem::path inputPath = path;
        const std::filesystem::path fullyQualifiedPath = isExternal ? inputPath :  m_assetDirectory / inputPath;
        const std::filesystem::path fullyQualifiedParentDirectory = fullyQualifiedPath.parent_path();

        // Need to pass in fully qualifed path to the nca
        std::ifstream file{fullyQualifiedPath};
        if(!file.is_open())
            throw nc::NcError("Could not open file: " + fullyQualifiedPath.string());

        // Need to pass in fully qualified path minus Church001.nca
        auto faces = ReadFacePathsFromAsset(file, fullyQualifiedParentDirectory);

        std::array<stbi_uc*, 6> pixelArray = {};
        int32_t width, height, numChannels; // Same for all faces.

        /** Front face */
        pixelArray.at(0) = stbi_load(faces.frontPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(0)) throw nc::NcError("Failed to load texture file: " + faces.frontPath);

        /** Back face */
        pixelArray.at(1) = stbi_load(faces.backPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(1)) throw nc::NcError("Failed to load texture file: " + faces.backPath);

        /** Up face */
        pixelArray.at(2) = stbi_load(faces.upPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(2)) throw nc::NcError("Failed to load texture file: " + faces.upPath);

        /** Down face */
        pixelArray.at(3) = stbi_load(faces.downPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(3)) throw nc::NcError("Failed to load texture file: " + faces.downPath);

        /** Right face */
        pixelArray.at(4) = stbi_load(faces.rightPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(4)) throw nc::NcError("Failed to load texture file: " + faces.rightPath);

        /** Left face */
        pixelArray.at(5) = stbi_load(faces.leftPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(5)) throw nc::NcError("Failed to load texture file: " + faces.leftPath);

        m_cubeMapData.emplace_back(pixelArray, width, height, width * height * STBI_rgb_alpha * 6, path);
        idsToLoad.push_back(path);
    }

    if (!idsToLoad.empty())
    {
        m_onUpdate.Emit
        (
            CubeMapBufferData(UpdateAction::Load, std::move(idsToLoad), std::span<const CubeMapData>{m_cubeMapData.begin() + newCubeMapIndex, m_cubeMapData.end()})
        );
    }
    return true;
}

bool CubeMapAssetManager::Unload(const std::string& path)
{
    auto pos = std::ranges::find_if(m_cubeMapData, [&uid = path](const auto& cubeMap)
    {
        return cubeMap.id == uid;
    });

    if (pos == m_cubeMapData.cend())
    {
        return false;
    }

    m_cubeMapData.erase(pos);

    m_onUpdate.Emit
    (
        CubeMapBufferData(UpdateAction::Unload, std::vector<std::string>{path}, std::span<const CubeMapData>{})
    );
    return true;
}

void CubeMapAssetManager::UnloadAll()
{
    m_cubeMapData.clear();
}

auto CubeMapAssetManager::Acquire(const std::string& path) const -> CubeMapView
{
    const auto pos = std::ranges::find_if(m_cubeMapData, [&path](const auto& data)
    {
        return data.id == path;
    });

    if(pos == m_cubeMapData.cend())throw NcError("Asset is not loaded: " + path);

    auto index = static_cast<uint32_t>(std::distance(m_cubeMapData.cbegin(), pos));
    return CubeMapView{CubeMapUsage::Skybox, index};
}

bool CubeMapAssetManager::IsLoaded(const std::string& path) const
{
    auto pos = std::ranges::find_if(m_cubeMapData, [&path](const auto& data)
    {
        return data.id == path;
    });

    return (pos != m_cubeMapData.end());
}

auto CubeMapAssetManager::OnUpdate() -> Signal<const CubeMapBufferData&>*
{
    return &m_onUpdate;
}
}