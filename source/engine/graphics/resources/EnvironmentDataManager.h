#pragma once

#include "ShaderResourceService.h"
#include "graphics/resources/ShaderDescriptorSets.h"
#include "graphics/resources/UniformBuffer.h"

namespace nc::graphics
{
    class Graphics;

    class EnvironmentDataManager : public IShaderResourceService<EnvironmentData>
    {
        public:
            EnvironmentDataManager(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors);
            ~EnvironmentDataManager() noexcept;

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