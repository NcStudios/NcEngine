#pragma once

#include "graphics/buffers/UniformBuffer.h"
#include "graphics/shaders/ShaderResourceService.h"
#include "graphics/shaders/ShaderDescriptorSets.h"

namespace nc::graphics
{
    class EnvironmentDataShaderResource : public IShaderResource<EnvironmentData>
    {
        public:
            EnvironmentDataShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors);
            ~EnvironmentDataShaderResource() noexcept;

            void Initialize() override;
            void Update(const std::vector<EnvironmentData>& data) override;
            void Reset() override;

        private:
            GpuAllocator* m_allocator;
            ShaderDescriptorSets* m_descriptors;
            std::unique_ptr<UniformBuffer> m_environmentDataBuffer;
            uint32_t m_bindingSlot;
    };
}