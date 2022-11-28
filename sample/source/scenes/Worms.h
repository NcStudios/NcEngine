#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class Worms : public Scene
{
    public:
        Worms(SampleUI* ui);
        void Load(Registry* registry, ModuleProvider modules) override;
};
}
