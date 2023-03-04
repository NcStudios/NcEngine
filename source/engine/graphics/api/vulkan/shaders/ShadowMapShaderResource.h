#pragma once

#include "ShaderDescriptorSets.h"
#include "graphics/api/vulkan/renderpasses/RenderPass.h"
#include "graphics/shader_resource/ShaderResourceService.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class GpuAllocator;

    struct ShadowMap
    {
        vk::ImageView imageView; 
    };

    class ShadowMapShaderResource : public IShaderResource<ShadowMap>
    {
        public:
            ShadowMapShaderResource(uint32_t bindingSlot, vk::Device device, ShaderDescriptorSets* descriptors, uint32_t maxShadows);

            void Initialize() override;
            void Update(const std::vector<ShadowMap>& data) override;
            void Reset() override;

        private:
            ShaderDescriptorSets* m_descriptors;
            vk::UniqueSampler m_sampler;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            uint32_t m_bindingSlot;
            uint32_t m_maxShadows;
    };
}