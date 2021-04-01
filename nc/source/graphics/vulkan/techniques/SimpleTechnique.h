#pragma once

#include "TechniqueBase.h"

#include "vulkan/vulkan.hpp"
#include <vector>

namespace nc::graphics::vulkan
{
    class Base; class Swapchain; class FrameManager;

    class SimpleTechnique : public TechniqueBase
    {
        public:
            SimpleTechnique(vulkan::FrameManager* frameManager);
            void Record();

        private:
            void CreatePipeline();
            void CreateRenderPasses();
    };
}