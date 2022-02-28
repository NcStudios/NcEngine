#pragma once

#include "ShaderResourceService.h"
#include "CubeMap.h"
#include "graphics/resources/shader_descriptor_sets.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    class CubeMapManager : public IShaderResourceService<CubeMap>
    {
        public:
            CubeMapManager(uint32_t bindingSlot, Graphics* graphics, shader_descriptor_sets* descriptors, uint32_t maxCubeMaps);
            ~CubeMapManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<CubeMap>& data) override;
            void Reset() override;

        private:
            Graphics* m_graphics;
            shader_descriptor_sets* m_descriptors;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            vk::UniqueSampler m_cubeMapSampler;
            vk::ImageLayout m_layout;
            uint32_t m_maxCubeMapsCount;
            bool m_cubeMapsInitialized;
            uint32_t m_bindingSlot;
    };
}