#pragma once

#include "ShaderResourceService.h"
#include "graphics/resources/UniformBuffer.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    class EnvironmentDataManager : public IShaderResourceService<EnvironmentData>
    {
        public:
            EnvironmentDataManager(Graphics* graphics);
            ~EnvironmentDataManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<EnvironmentData>& data) override;
            auto GetDescriptorSet() -> vk::DescriptorSet* override;
            auto GetDescriptorSetLayout() -> vk::DescriptorSetLayout* override;
            void Reset() override;

        private:
            Graphics* m_graphics;
            std::unique_ptr<UniformBuffer> m_environmentDataBuffer;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
    };
}