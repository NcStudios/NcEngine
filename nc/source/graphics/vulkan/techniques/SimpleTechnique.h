#pragma once

#include "TechniqueBase.h"
#include "vulkan/vulkan.hpp"
#include <vector>

namespace nc::graphics::vulkan
{
    class Base; class Swapchain; class Commands; struct GlobalData;

    class SimpleTechnique : public TechniqueBase
    {
        public:
            SimpleTechnique(const GlobalData& globalData);
            ~SimpleTechnique();
            void Record(Commands* commands) override;

        private:
            void CreateDescriptorSetLayout();
            void CreatePipeline();
            void CreateRenderPasses();

            vk::DescriptorSetLayout m_descriptorSetLayout;
    };
}