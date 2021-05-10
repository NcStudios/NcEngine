#pragma once

#include "TechniqueBase.h"
#include "vulkan/vulkan.hpp"
#include <vector>

namespace nc::graphics
{
    namespace vulkan
    {
        class Graphics2;
        class Base; class Swapchain; class Commands; struct GlobalData;

        class PhongAndUiTechnique : public TechniqueBase
        {
            public:
                PhongAndUiTechnique(GlobalData* globalData, nc::graphics::Graphics2* graphics);
                ~PhongAndUiTechnique() noexcept;
                void Record(Commands* commands) override;

            private:
                void CreateDescriptorSetLayout();
                void CreatePipeline();
                void CreateRenderPasses();

                vk::DescriptorSetLayout m_descriptorSetLayout;
        };
    }
}