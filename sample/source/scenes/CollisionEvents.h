#pragma once

#include "scene/Scene.h"
#include "shared/SampleUI.h"

namespace nc::sample
{
    class CollisionEvents : public Scene
    {
        public:
            CollisionEvents(SampleUI* ui);
            void Load(Registry* registry, ModuleProvider modules) override;
    };
}