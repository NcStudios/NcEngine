#pragma once

#include "scene/Scene.h"
#include "shared/SampleUI.h"

namespace nc::sample
{
    class SpawnTest : public Scene
    {
        public:
            SpawnTest(SampleUI* ui);
            void Load(Registry* registry, ModuleProvider modules) override;
    };
}