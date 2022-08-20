#pragma once

#include "RenderTarget.h"
#include "ShaderResourceService.h"
#include "graphics/resources/ShaderDescriptorSets.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    struct ShadowMap
    {
        Vector2 dimensions;
    };

    class ShadowMapManager : public IShaderResourceService<ShadowMap>
    {
        public:
            ShadowMapManager(uint32_t bindingSlot, Graphics* graphics, ShaderDescriptorSets* descriptors, Vector2 dimensions);
            ~ShadowMapManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<ShadowMap>& data) override;
            void Reset() override;

            auto GetImageView() noexcept -> const vk::ImageView& { return m_depthStencil.get()->GetImageView(); }

        private:
            Graphics* m_graphics;
            ShaderDescriptorSets* m_descriptors;
            vk::UniqueSampler m_sampler;
            std::unique_ptr<RenderTarget> m_depthStencil;
            Vector2 m_dimensions;
            uint32_t m_bindingSlot;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            bool m_isRegistered;
    };
}