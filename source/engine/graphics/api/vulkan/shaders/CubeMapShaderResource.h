#pragma once

#include "ShaderDescriptorSets.h"
#include "graphics/api/vulkan/cubemaps/CubeMap.h"
#include "graphics/shader_resource/ShaderResourceService.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class CubeMapShaderResource : public IShaderResource<CubeMap>
{
    public:
        CubeMapShaderResource(uint32_t bindingSlot, vk::Device device, ShaderDescriptorSets* descriptors, uint32_t maxCubeMaps);

        void Initialize() override;
        void Update(uint32_t frameIndex, const std::vector<CubeMap>& data) override;
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