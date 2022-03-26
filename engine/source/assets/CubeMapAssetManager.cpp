#include "CubeMapAssetManager.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"
#include "graphics/resources/ShaderResourceService.h"

#include <cassert>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace nc
{
    CubeMapAssetManager::CubeMapAssetManager(graphics::Graphics* graphics, const std::string& cubeMapAssetDirectory, uint32_t maxCubeMapsCount)
        : m_cubeMapAccessors{},
          m_cubeMaps{},
          m_graphics{graphics},
          m_assetDirectory{cubeMapAssetDirectory},
          m_cubeMapSampler{m_graphics->GetBasePtr()->CreateTextureSampler()},
          m_maxCubeMapsCount{maxCubeMapsCount}
    {
    }

    CubeMapAssetManager::~CubeMapAssetManager() noexcept
    {
        for(auto& cubeMap : m_cubeMaps)
        {
            cubeMap.Clear();
        }
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
            else if (currentFacePath.stem().string() == "back") facePaths.backPath   = (parentPath / currentFacePath).string();
            else if (currentFacePath.stem().string() == "up") facePaths.upPath       = (parentPath / currentFacePath).string();
            else if (currentFacePath.stem().string() == "down") facePaths.downPath   = (parentPath / currentFacePath).string();
            else if (currentFacePath.stem().string() == "right") facePaths.rightPath = (parentPath / currentFacePath).string();
            else if (currentFacePath.stem().string() == "left") facePaths.leftPath   = (parentPath / currentFacePath).string();
        }

        return facePaths;
    }

    bool HasValidAssetExtension(const std::string& path)
    {
        std::size_t periodPosition = path.rfind('.');
        const std::string fileExtension = path.substr(periodPosition+1);
        return fileExtension.compare("nca") == 0 ? true : false;
    }


    /** If external (from the editor) path will be: D:\\NC\\Projects\\Test\\assets\\Textures\\Cubemaps\\Church001\\Church001.nca"
     *  If internal (called programmatically) path will be: Church001\\Church001.nca"
     * */
    bool CubeMapAssetManager::Load(const std::string& path, bool isExternal)
    {
        uint32_t nextCubeMapIndex = static_cast<uint32_t>(m_cubeMaps.size());

        if(IsLoaded(path))
            return true;

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

        m_cubeMapAccessors.emplace(path, cubeMapView);
        m_cubeMaps.emplace_back(m_graphics->GetBasePtr(), m_graphics->GetAllocatorPtr(), pixelArray, width, height, width * height * STBI_rgb_alpha * 6, path);
        graphics::ShaderResourceService<graphics::CubeMap>::Get()->Update(m_cubeMaps);
        return true;
    }

    bool CubeMapAssetManager::Load(std::span<const std::string> paths, bool isExternal)
    {
        const auto newCubeMapCount = paths.size();
        uint32_t newCubeMapIndex = static_cast<uint32_t>(m_cubeMaps.size());
        if (newCubeMapCount + newCubeMapIndex >= m_maxCubeMapsCount)
            throw NcError("Cannot exceed max texture count.");

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

            auto cubeMapView = CubeMapView
            {
                .usage = CubeMapUsage::Skybox,
                .index = newCubeMapIndex++
            };

            m_cubeMapAccessors.emplace(path, cubeMapView);
            m_cubeMaps.emplace_back(m_graphics->GetBasePtr(), m_graphics->GetAllocatorPtr(), pixelArray, width, height, width * height * STBI_rgb_alpha * 6, path);
        }

        graphics::ShaderResourceService<graphics::CubeMap>::Get()->Update(m_cubeMaps);
        return true;
    }

    bool CubeMapAssetManager::Unload(const std::string& path)
    {
        auto removed = static_cast<bool>(m_cubeMapAccessors.erase(path));
        if(!removed)
            return false;
        
        auto pos = std::ranges::find_if(m_cubeMaps, [&uid = path](const auto& cubeMap)
        {
            return cubeMap.GetUid() == uid;
        });

        assert(pos != m_cubeMaps.end());
        auto index = std::distance(m_cubeMaps.begin(), pos);
        m_cubeMaps.erase(pos);

        for(auto& [cubeMapPath, cubeMapView] : m_cubeMapAccessors)
        {
            if(cubeMapView.index > index)
                --cubeMapView.index;
        }

        graphics::ShaderResourceService<graphics::CubeMap>::Get()->Update(m_cubeMaps);
        return true;
    }

    void CubeMapAssetManager::UnloadAll()
    {
        m_cubeMapAccessors.clear();
        m_cubeMaps.clear();
    }

    auto CubeMapAssetManager::Acquire(const std::string& path) const -> CubeMapView
    {
        const auto it = m_cubeMapAccessors.find(path);
        if(it == m_cubeMapAccessors.cend())
            throw NcError("Asset is not loaded: " + path);
        
        return it->second;
    }
    
    bool CubeMapAssetManager::IsLoaded(const std::string& path) const
    {
        return m_cubeMapAccessors.contains(path);
    }
}