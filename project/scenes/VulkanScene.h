#pragma once

#include "Scene.h"

namespace project
{
    class VulkanScene : public nc::scene::Scene
    {
        public:
            void Load() override;
            void Unload() override;
    };
}