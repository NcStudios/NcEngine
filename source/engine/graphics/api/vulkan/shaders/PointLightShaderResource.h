#pragma once

#include "ShaderDescriptorSets.h"
#include "graphics/api/vulkan/buffers/WriteableBuffer.h"
#include "graphics/shader_resource/PointLightData.h"
#include "graphics/shader_resource/ShaderResourceService.h"

namespace nc::graphics
{
    class PointLightShaderResource : public IShaderResource<PointLightData>
    {
        public:
            PointLightShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, uint32_t maxPointLights);
            ~PointLightShaderResource() noexcept;

            void Initialize() override;
            void Update(const std::vector<PointLightData>& data) override;
            void Reset() override;

        private:
            std::unique_ptr<WriteableBuffer<PointLightData>> m_pointLightsArrayBuffer;
            ShaderDescriptorSets* m_descriptors;
            GpuAllocator* m_allocator;
            uint32_t m_maxPointLights;
            uint32_t m_bindingSlot;
    };
}