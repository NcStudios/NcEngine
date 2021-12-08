#pragma once

#include "ShaderResourceService.h"
#include "CubeMap.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    class CubeMapManager : public IShaderResourceService<CubeMap>
    {
        public:
            CubeMapManager(Graphics* graphics, uint32_t maxCubeMaps);
            ~CubeMapManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<CubeMap>& data) override;
            auto GetDescriptorSet() -> vk::DescriptorSet* override;
            auto GetDescriptorSetLayout() -> vk::DescriptorSetLayout* override;
            void Reset() override;

        private:
            Graphics* m_graphics;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
            vk::UniqueSampler m_cubeMapSampler;
            vk::ImageLayout m_layout;
            uint32_t m_maxCubeMapsCount;
            bool m_cubeMapsInitialized;
    };
}