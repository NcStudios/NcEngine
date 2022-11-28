#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class SolarSystem : public Scene
{
    public:
        SolarSystem(SampleUI* ui);
        void Load(Registry* registry, ModuleProvider modules) override;
};
}
