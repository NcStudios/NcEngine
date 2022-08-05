#pragma once

#include "scene/Scene.h"
#include "shared/SampleUI.h"

namespace nc::sample
{
    class SolarSystem : public Scene
    {
    public:
        SolarSystem(SampleUI* ui);
        void Load(Registry* registry, ModuleProvider modules) override;
    };
}