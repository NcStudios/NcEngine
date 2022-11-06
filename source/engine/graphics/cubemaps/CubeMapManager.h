#pragma once

#include "graphics/vk/CubeMap.h"
#include "graphics/resources/ShaderDescriptorSets.h"
#include "graphics/resources/ShaderResourceService.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class CubeMapManager : public IShaderResourceService<CubeMap>
{
    public:
        CubeMapManager(vk::Device device, uint32_t bindingSlot, ShaderDescriptorSets* descriptors, uint32_t maxCubeMaps);

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