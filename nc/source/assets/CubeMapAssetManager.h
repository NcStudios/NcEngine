#pragma once

#include "assets/AssetService.h"
#include "graphics/resources/CubeMap.h"

#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace nc
{
    namespace graphics
    {
        class Graphics;
        struct Texture;
    }

    class CubeMapAssetManager : public IAssetService<CubeMapView, CubeMapFaces>
    {
        public:
            CubeMapAssetManager(graphics::Graphics* graphics, const std::string& cubeMapAssetDirectory, uint32_t maxCubeMapsCount);
            ~CubeMapAssetManager() noexcept;

            bool Load(const CubeMapFaces& faces, bool isExternal) override;
            bool Load(std::span<const CubeMapFaces> facesSet, bool isExternal) override;
            bool Unload(const CubeMapFaces& faces) override;
            void UnloadAll() override;
            auto Acquire(const CubeMapFaces& faces) const -> CubeMapView override;
            bool IsLoaded(const CubeMapFaces& faces) const override;

        private:
            std::unordered_map<std::string, CubeMapView> m_cubeMapAccessors;
            std::vector<graphics::CubeMap> m_cubeMaps;
            graphics::Graphics* m_graphics;
            std::string m_assetDirectory;
            vk::UniqueSampler m_cubeMapSampler;
            uint32_t m_maxCubeMapsCount;
    };
}