#pragma once

#include "ShaderResourceService.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    struct EnvironmentData
    {
    };

    class EnvironmentDataManager : public IShaderResourceService<ShadowMap>
    {
        public:
            EnvironmentDataManager(Graphics* graphics, Vector2 dimensions);
            ~EnvironmentDataManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<ShadowMap>& data) override;
            auto GetDescriptorSet() -> vk::DescriptorSet* override;
            auto GetDescriptorSetLayout() -> vk::DescriptorSetLayout* override;
            void Reset() override;

            auto GetImageView() noexcept -> const vk::ImageView& { return m_depthStencil.get()->GetImageView(); } 

        private:
            Graphics* m_graphics;
            vk::UniqueSampler m_sampler;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
            Vector2 m_dimensions;
    };
}