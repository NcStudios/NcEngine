#pragma once

#include "ShaderResourceService.h"
#include "WriteableBuffer.h"
#include "component/PointLight.h"

namespace nc::graphics
{
    class PointLightManager : public IShaderResourceService<PointLightInfo>
    {
        public:
            PointLightManager(Graphics* graphics, uint32_t maxPointLights);
            ~PointLightManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<PointLightInfo>& data) override;
            auto GetDescriptorSet() -> vk::DescriptorSet* override;
            auto GetDescriptorSetLayout() -> vk::DescriptorSetLayout* override;
            void Reset() override;

        private:
            WriteableBuffer<nc::PointLightInfo> m_pointLightsArrayBuffer;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
            Graphics* m_graphics;
            uint32_t m_maxPointLights;
    };
}