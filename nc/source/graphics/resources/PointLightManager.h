#pragma once

#include "ShaderResourceService.h"
#include "WriteableBuffer.h"
#include "ecs/component/PointLight.h"
#include "graphics/resources/shader_descriptor_sets.h"

namespace nc::graphics
{
    class PointLightManager : public IShaderResourceService<PointLightInfo>
    {
        public:
            PointLightManager(uint32_t bindingSlot, GpuAllocator* allocator, shader_descriptor_sets* descriptors, uint32_t maxPointLights);
            ~PointLightManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<PointLightInfo>& data) override;
            void Reset() override;

        private:
            std::unique_ptr<WriteableBuffer<nc::PointLightInfo>> m_pointLightsArrayBuffer;
            shader_descriptor_sets* m_descriptors;
            GpuAllocator* m_allocator;
            uint32_t m_maxPointLights;
            uint32_t m_bindingSlot;
    };
}