#pragma once

#include "ShaderResourceService.h"
#include "WriteableBuffer.h"
#include "ecs/component/PointLight.h"
#include "graphics/resources/ShaderDescriptorSets.h"

namespace nc::graphics
{
    class PointLightManager : public IShaderResourceService<PointLightInfo>
    {
        public:
            PointLightManager(uint32_t bindingSlot, Graphics* graphics, ShaderDescriptorSets* descriptors, uint32_t maxPointLights);
            ~PointLightManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<PointLightInfo>& data) override;
            void Reset() override;

        private:
            std::unique_ptr<WriteableBuffer<nc::PointLightInfo>> m_pointLightsArrayBuffer;
            ShaderDescriptorSets* m_descriptors;
            Graphics* m_graphics;
            uint32_t m_maxPointLights;
            uint32_t m_bindingSlot;
    };
}