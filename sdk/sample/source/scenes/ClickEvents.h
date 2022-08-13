#pragma once

#include "scene/Scene.h"
#include "shared/SampleUI.h"

namespace nc::sample
{
    class ClickEvents : public Scene
    {
        public:
            ClickEvents(SampleUI* ui);
            void Load(Registry* registry, ModuleProvider modules) override;
    };
}