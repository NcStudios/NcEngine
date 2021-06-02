#pragma once

#include "Scene.h"

namespace nc::sample
{
    class VulkanScene : public scene::Scene
    {
        public:
            void Load(registry_type* registry) override;
            void Unload() override;
    };
}