#pragma once

#include "scene/Scene.h"
#include "shared/SampleUI.h"

namespace nc::sample
{
    class JointsTest : public Scene
    {
        public:
            JointsTest(SampleUI* ui);
            void Load(Registry* registry, ModuleProvider modules) override;
    };
}