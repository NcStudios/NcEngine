#pragma once

#include "graphics/resources/WriteableBuffer.h"
#include "graphics/resources/PointLight.h"
#include "graphics/shaders/ShaderDescriptorSets.h"
#include "graphics/shaders/ShaderResourceService.h"

namespace nc::graphics
{
    class PointLightShaderResource : public IShaderResourceService<PointLightInfo>
    {
        public:
            PointLightShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, uint32_t maxPointLights);
            ~PointLightShaderResource() noexcept;

            void Initialize() override;
            void Update(const std::vector<PointLightInfo>& data) override;
            void Reset() override;

        private:
            std::unique_ptr<WriteableBuffer<PointLightInfo>> m_pointLightsArrayBuffer;
            ShaderDescriptorSets* m_descriptors;
            GpuAllocator* m_allocator;
            uint32_t m_maxPointLights;
            uint32_t m_bindingSlot;
    };
}