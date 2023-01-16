#pragma once

#include "ecs/Registry.h"
#include "graphics/renderpasses/RenderTarget.h"
#include "graphics/shaders/ShaderDescriptorSets.h"
#include "graphics/shaders/ShaderResourceService.h"
#include "utility/Signal.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class GpuAllocator; class PointLight;

    struct ShadowMap
    {
        Vector2 dimensions;
    };

    class ShadowMapShaderResource : public IShaderResource<ShadowMap>
    {
        public:
            ShadowMapShaderResource(uint32_t bindingSlot, vk::Device device, Registry* registry, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, Vector2 dimensions);
            ~ShadowMapShaderResource() noexcept;

            void Initialize() override;
            void Update(const std::vector<ShadowMap>& data) override;
            void Reset() override;

            auto GetImageView(uint32_t index) noexcept -> vk::ImageView{return m_depthStencils[index].get()->GetImageView();}

        private:
            void AddShadowMapResource();
            void RemoveShadowMapResource();

            vk::Device m_device;
            GpuAllocator* m_allocator;
            ShaderDescriptorSets* m_descriptors;
            std::vector<vk::UniqueSampler> m_samplers;
            std::vector<std::unique_ptr<RenderTarget>> m_depthStencils;
            Vector2 m_dimensions;
            uint32_t m_bindingSlot;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            Connection<PointLight&> m_onAddPointLightConnection;
            Connection<Entity> m_onRemovePointLightConnection;
            uint32_t m_numShadowCasters;
            bool m_isRegistered;
    };
}