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

        class SimpleTechnique : public TechniqueBase
        {
            public:
                SimpleTechnique(const GlobalData& globalData, nc::graphics::Graphics2* graphics);
                ~SimpleTechnique() noexcept;
                void Record(Commands* commands) override;

            private:
                void CreateDescriptorSetLayout();
                void CreatePipeline();
                void CreateRenderPasses();

                vk::DescriptorSetLayout m_descriptorSetLayout;
        };
    }
}