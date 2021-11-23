#pragma once

#include "ShaderResourceService.h"
#include "graphics/resources/ImmutableBuffer.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    class SceneDataManager : public IShaderResourceService<SceneData>
    {
        public:
            SceneDataManager(Graphics* graphics);
            ~SceneDataManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<SceneData>& data) override;
            auto GetDescriptorSet() -> vk::DescriptorSet* override;
            auto GetDescriptorSetLayout() -> vk::DescriptorSetLayout* override;
            void Reset() override;

        private:
            Graphics* m_graphics;
            std::unique_ptr<ImmutableBuffer> m_sceneDataBuffer;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
    };
}