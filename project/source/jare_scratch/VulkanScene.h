#pragma once

#include "Scene.h"

namespace nc::sample
{
    class VulkanScene : public scene::Scene
    {
        public:
            void Load() override;
            void Unload() override;
    };
}