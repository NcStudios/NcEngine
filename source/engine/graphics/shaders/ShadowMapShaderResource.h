#pragma once

#include "graphics/renderpasses/RenderTarget.h"
#include "graphics/shaders/ShaderDescriptorSets.h"
#include "graphics/shaders/ShaderResourceService.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class GpuAllocator;

    struct ShadowMap
    {
        Vector2 dimensions;
    };

    class ShadowMapShaderResource : public IShaderResource<ShadowMap>
    {
        public:
            ShadowMapShaderResource(vk::Device device, uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, Vector2 dimensions, uint32_t numLights);
            ~ShadowMapShaderResource() noexcept;

            void Initialize() override;
            void Update(const std::vector<ShadowMap>& data) override;
            void Reset() override;

            auto GetImageView(uint32_t index) noexcept -> vk::ImageView{return m_depthStencils[index].get()->GetImageView();}

        private:
            vk::Device m_device;
            GpuAllocator* m_allocator;
            ShaderDescriptorSets* m_descriptors;
            uint32_t m_numLights;
            std::vector<vk::UniqueSampler> m_samplers;
            std::vector<std::unique_ptr<RenderTarget>> m_depthStencils;
            Vector2 m_dimensions;
            uint32_t m_bindingSlot;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            bool m_isRegistered;
    };
}