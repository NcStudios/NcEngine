#pragma once

#include "TechniqueBase.h"
#include "vulkan/vulkan.hpp"
#include <vector>
#include "directx/math/DirectXMath.h"

namespace nc::graphics
{
    namespace vulkan
    {
        class Graphics2;
        class Base; class Swapchain; class Commands; struct GlobalData;

        struct PushConstants
        {
            DirectX::XMMATRIX modelView;
            DirectX::XMMATRIX model;
            uint32_t baseColorIndex;
            uint32_t normalColorIndex;
            uint32_t roughnessColorIndex;
        };

        class PhongAndUiTechnique : public TechniqueBase
        {
            public:
                PhongAndUiTechnique(nc::graphics::Graphics2* graphics);
                ~PhongAndUiTechnique() noexcept;
                void Record(Commands* commands) override;

            private:
                void CreatePipeline();
                void CreateRenderPasses();

                vk::DescriptorSetLayout m_descriptorSetLayout;
                vk::DescriptorSet m_textureDescriptors;
        };
    }
}