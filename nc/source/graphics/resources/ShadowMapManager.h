#pragma once

#include "DepthStencil.h"
#include "ShaderResourceService.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    class Tag
    {
        std::string tag;
    };

    class ShadowMapManager : public IShaderResourceService<Tag>
    {
        public:
            ShadowMapManager(Graphics* graphics, Vector2 dimensions);
            ~ShadowMapManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<Tag>& data) override;
            auto GetDescriptorSet() -> vk::DescriptorSet* override;
            auto GetDescriptorSetLayout() -> vk::DescriptorSetLayout* override;
            void Reset() override;

            auto GetImageView() noexcept -> const vk::ImageView& { return m_depthStencil.get()->GetImageView(); } 

        private:
            Graphics* m_graphics;
            vk::UniqueSampler m_sampler;
            std::unique_ptr<DepthStencil> m_depthStencil;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
            Vector2 m_dimensions;
    };
}