#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class SpawnTest : public Scene
{
    public:
        SpawnTest(SampleUI* ui);
        void Load(Registry* registry, ModuleProvider modules) override;
};
}
