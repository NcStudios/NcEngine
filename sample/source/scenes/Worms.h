#pragma once

#include "scene/Scene.h"
#include "shared/SampleUI.h"

namespace nc::sample
{
    class Worms : public Scene
    {
        public:
            Worms(SampleUI* ui);
            void Load(Registry* registry, ModuleProvider modules) override;
    };
}