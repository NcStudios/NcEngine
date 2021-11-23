#include "CubeMapAssetManager.h"
#include "graphics/Initializers.h"
#include "graphics/resources/ShaderResourceService.h"


#include <cassert>
#include <fstream>

namespace nc
{
    CubeMapAssetManager::CubeMapAssetManager(graphics::Graphics* graphics, uint32_t maxCubeMapsCount)
        : m_cubeMapAccessors{},
          m_cubeMaps{},
          m_graphics{graphics},
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

    bool CubeMapAssetManager::Load(const CubeMapFaces& faces)
    {
        auto nextCubeMapIndex = m_cubeMaps.size();

        if(IsLoaded(faces))
            return true;

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
            .usage = faces.usage,
            .index = nextCubeMapIndex++
        };

        m_cubeMapAccessors.emplace(faces.uid, cubeMapView);
        m_cubeMaps.emplace_back(m_graphics, pixelArray, width, height, width * height * STBI_rgb_alpha * 6);
        graphics::ShaderResourceService<graphics::CubeMap>::Get()->Update(m_cubeMaps);
        return true;
    }

    bool CubeMapAssetManager::Load(std::span<const CubeMapFaces> facesSet)
    {
        const auto newCubeMapCount = facesSet.size();
        auto newCubeMapIndex = m_cubeMaps.size();
        if (newCubeMapCount + newCubeMapIndex >= m_maxCubeMapsCount)
            throw NcError("Cannot exceed max texture count.");

        for (const auto& faces : facesSet)
        {
            if(IsLoaded(faces))
                continue;

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
                .usage = faces.usage,
                .index = newCubeMapIndex++
            };

            m_cubeMapAccessors.emplace(faces.uid, cubeMapView);
            m_cubeMaps.emplace_back(m_graphics, pixelArray, width, height, width * height * STBI_rgb_alpha * 6);
        }

        graphics::ShaderResourceService<graphics::CubeMap>::Get()->Update(m_cubeMaps);
        return true;
    }

    bool CubeMapAssetManager::Unload(const CubeMapFaces& faces)
    {
        // auto removed = static_cast<bool>(m_cubeMapAccessors.erase(faces.uid));
        // if(!removed)
        //     return false;
        
        // auto pos = std::ranges::find_if(m_cubeMaps, [&uid = faces.uid](const auto& cubeMap)
        // {
        //     return cubeMap. == uid;
        // });

        // assert(pos != m_textures.end());
        // auto index = std::distance(m_textures.begin(), pos);
        // m_textures.erase(pos);

        // for(auto& [path, textureView] : m_accessors)
        // {
        //     if(textureView.index > index)
        //         --textureView.index;
        // }

        // graphics::ShaderResourceService<graphics::Texture>::Get()->Update(m_textures);
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