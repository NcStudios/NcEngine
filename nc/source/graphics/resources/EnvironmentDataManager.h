#pragma once

#include "ShaderResourceService.h"
#include "graphics/resources/shader_descriptor_sets.h"
#include "graphics/resources/UniformBuffer.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    class EnvironmentDataManager : public IShaderResourceService<EnvironmentData>
    {
        public:
            EnvironmentDataManager(uint32_t bindingSlot, Graphics* graphics, shader_descriptor_sets* descriptors);
            ~EnvironmentDataManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<EnvironmentData>& data) override;
            void Reset() override;

        private:
            Graphics* m_graphics;
            shader_descriptor_sets* m_descriptors;
            std::unique_ptr<UniformBuffer> m_environmentDataBuffer;
            uint32_t m_bindingSlot;
    };
}