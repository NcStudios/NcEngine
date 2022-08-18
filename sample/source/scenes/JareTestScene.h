#pragma once

#include "scene/Scene.h"
#include "shared/SampleUI.h"

namespace nc::sample
{
    class JareTestScene : public Scene
    {
        public:
            JareTestScene(SampleUI* ui);
            void Load(Registry* registry, ModuleProvider modules) override;
    };
}