#pragma once

#include "graphics/cubemaps/CubeMap.h"
#include "graphics/shaders/ShaderDescriptorSets.h"
#include "graphics/shaders/ShaderResourceService.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class CubeMapShaderResource : public IShaderResource<CubeMap>
{
    public:
        CubeMapShaderResource(vk::Device device, uint32_t bindingSlot, ShaderDescriptorSets* descriptors, uint32_t maxCubeMaps);

        void Initialize() override;
        void Update(const std::vector<CubeMap>& data) override;
        void Reset() override;

    private:
        vk::Device m_device;
        ShaderDescriptorSets* m_descriptors;
        std::vector<vk::DescriptorImageInfo> m_imageInfos;
        vk::UniqueSampler m_cubeMapSampler;
        vk::ImageLayout m_layout;
        uint32_t m_maxCubeMapsCount;
        bool m_cubeMapsInitialized;
        uint32_t m_bindingSlot;
};
} // namespace nc::graphics