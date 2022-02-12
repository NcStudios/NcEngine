#pragma once

#include "ShaderResourceService.h"
#include "CubeMap.h"
#include "graphics/resources/ShaderDescriptorSets.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    class CubeMapManager : public IShaderResourceService<CubeMap>
    {
        public:
            CubeMapManager(Graphics* graphics, ShaderDescriptorSets* descriptors, uint32_t maxCubeMaps);
            ~CubeMapManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<CubeMap>& data) override;
            void Reset() override;

        private:
            Graphics* m_graphics;
            ShaderDescriptorSets* m_descriptors;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            vk::UniqueSampler m_cubeMapSampler;
            vk::ImageLayout m_layout;
            uint32_t m_maxCubeMapsCount;
            bool m_cubeMapsInitialized;
    };
}