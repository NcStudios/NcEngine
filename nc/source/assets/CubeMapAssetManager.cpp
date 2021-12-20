#include "CubeMapAssetManager.h"
#include "graphics/Initializers.h"
#include "graphics/resources/ShaderResourceService.h"


#include <cassert>
#include <fstream>

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

    bool CubeMapAssetManager::Load(const CubeMapFaces& faces, bool isExternal)
    {
        uint32_t nextCubeMapIndex = static_cast<uint32_t>(m_cubeMaps.size());

        if(IsLoaded(faces))
            return true;

        std::array<stbi_uc*, 6> pixelArray = {};
        int32_t width, height, numChannels; // Same for all faces.

        /** Front face */
        const auto fullFrontPath = isExternal ? faces.frontPath : m_assetDirectory + faces.frontPath;
        pixelArray.at(0) = stbi_load(fullFrontPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(0)) throw nc::NcError("Failed to load texture file: " + fullFrontPath);

        /** Back face */
        const auto fullBackPath = isExternal ? faces.backPath : m_assetDirectory + faces.backPath;
        pixelArray.at(1) = stbi_load(fullBackPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(1)) throw nc::NcError("Failed to load texture file: " + fullBackPath);

        /** Up face */
        const auto fullUpPath = isExternal ? faces.upPath : m_assetDirectory + faces.upPath;
        pixelArray.at(2) = stbi_load(fullUpPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(2)) throw nc::NcError("Failed to load texture file: " + fullUpPath);

        /** Down face */
        const auto fullDownPath = isExternal ? faces.downPath : m_assetDirectory + faces.downPath;
        pixelArray.at(3) = stbi_load(fullDownPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(3)) throw nc::NcError("Failed to load texture file: " + fullDownPath);

        /** Right face */
        const auto fullRightPath = isExternal ? faces.rightPath : m_assetDirectory + faces.rightPath;
        pixelArray.at(4) = stbi_load(fullRightPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(4)) throw nc::NcError("Failed to load texture file: " + fullRightPath);

        /** Left face */
        const auto fullLeftPath = isExternal ? faces.leftPath : m_assetDirectory + faces.leftPath;
        pixelArray.at(5) = stbi_load(fullLeftPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if(!pixelArray.at(5)) throw nc::NcError("Failed to load texture file: " + fullLeftPath);

        auto cubeMapView = CubeMapView
        {
            .usage = faces.usage,
            .index = nextCubeMapIndex++
        };

        m_cubeMapAccessors.emplace(faces.uid, cubeMapView);
        m_cubeMaps.emplace_back(m_graphics, pixelArray, width, height, width * height * STBI_rgb_alpha * 6, faces.uid);
        graphics::ShaderResourceService<graphics::CubeMap>::Get()->Update(m_cubeMaps);
        return true;
    }

    bool CubeMapAssetManager::Load(std::span<const CubeMapFaces> facesSet, bool isExternal)
    {
        const auto newCubeMapCount = facesSet.size();
        uint32_t newCubeMapIndex = static_cast<uint32_t>(m_cubeMaps.size());
        if (newCubeMapCount + newCubeMapIndex >= m_maxCubeMapsCount)
            throw NcError("Cannot exceed max texture count.");

        for (const auto& faces : facesSet)
        {
            if(IsLoaded(faces))
                continue;

            std::array<stbi_uc*, 6> pixelArray = {};
            int32_t width, height, numChannels; // Same for all faces.

            /** Front face */
            const auto fullFrontPath = isExternal ? faces.frontPath : m_assetDirectory + faces.frontPath;
            pixelArray.at(0) = stbi_load(fullFrontPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
            if(!pixelArray.at(0)) throw nc::NcError("Failed to load texture file: " + fullFrontPath);

            /** Back face */
            const auto fullBackPath = isExternal ? faces.backPath : m_assetDirectory + faces.backPath;
            pixelArray.at(1) = stbi_load(fullBackPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
            if(!pixelArray.at(1)) throw nc::NcError("Failed to load texture file: " + fullBackPath);

            /** Up face */
            const auto fullUpPath = isExternal ? faces.upPath : m_assetDirectory + faces.upPath;
            pixelArray.at(2) = stbi_load(fullUpPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
            if(!pixelArray.at(2)) throw nc::NcError("Failed to load texture file: " + fullUpPath);

            /** Down face */
            const auto fullDownPath = isExternal ? faces.downPath : m_assetDirectory + faces.downPath;
            pixelArray.at(3) = stbi_load(fullDownPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
            if(!pixelArray.at(3)) throw nc::NcError("Failed to load texture file: " + fullDownPath);

            /** Right face */
            const auto fullRightPath = isExternal ? faces.rightPath : m_assetDirectory + faces.rightPath;
            pixelArray.at(4) = stbi_load(fullRightPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
            if(!pixelArray.at(4)) throw nc::NcError("Failed to load texture file: " + fullRightPath);

            /** Left face */
            const auto fullLeftPath = isExternal ? faces.leftPath : m_assetDirectory + faces.leftPath;
            pixelArray.at(5) = stbi_load(fullLeftPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
            if(!pixelArray.at(5)) throw nc::NcError("Failed to load texture file: " + fullLeftPath);
            
            auto cubeMapView = CubeMapView
            {
                .usage = faces.usage,
                .index = newCubeMapIndex++
            };

            m_cubeMapAccessors.emplace(faces.uid, cubeMapView);
            m_cubeMaps.emplace_back(m_graphics, pixelArray, width, height, width * height * STBI_rgb_alpha * 6, faces.uid);
        }

        graphics::ShaderResourceService<graphics::CubeMap>::Get()->Update(m_cubeMaps);
        return true;
    }

    bool CubeMapAssetManager::Unload(const CubeMapFaces& faces)
    {
        auto removed = static_cast<bool>(m_cubeMapAccessors.erase(faces.uid));
        if(!removed)
            return false;
        
        auto pos = std::ranges::find_if(m_cubeMaps, [&uid = faces.uid](const auto& cubeMap)
        {
            return cubeMap.GetUid() == uid;
        });

        assert(pos != m_cubeMaps.end());
        auto index = std::distance(m_cubeMaps.begin(), pos);
        m_cubeMaps.erase(pos);

        for(auto& [path, cubeMapView] : m_cubeMapAccessors)
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

    auto CubeMapAssetManager::Acquire(const CubeMapFaces& faces) const -> CubeMapView
    {
        const auto it = m_cubeMapAccessors.find(faces.uid);
        if(it == m_cubeMapAccessors.cend())
            throw NcError("Asset is not loaded: " + faces.uid);
        
        return it->second;
    }
    
    bool CubeMapAssetManager::IsLoaded(const CubeMapFaces& faces) const
    {
        return m_cubeMapAccessors.contains(faces.uid);
    }
}