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

    class CubeMapAssetManager : public IAssetService<CubeMapView>
    {
        public:
            CubeMapAssetManager(graphics::Graphics* graphics, uint32_t maxTextures);
            ~CubeMapAssetManager() noexcept;

            bool Load(const CubeMapFaces& paths) override;
            bool Load(std::span<const CubeMapFaces> paths) override;
            bool Unload(const CubeMapFaces& paths) override;
            void UnloadAll() override;
            auto Acquire(const CubeMapFaces& paths) const -> CubeMapView override;
            bool IsLoaded(const CubeMapFaces& paths) const override;

        private:
            std::unordered_map<std::string, CubeMapView> m_cubeMapAccessors;
            std::vector<graphics::CubeMap> m_cubeMaps;
            graphics::Graphics* m_graphics;
            vk::UniqueSampler m_cubeMapSampler;
            uint32_t m_maxTextureCount;
    };
}