#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class CollisionBenchmark : public Scene
{
    public:
        CollisionBenchmark(SampleUI* ui);
        void Load(Registry* registry, ModuleProvider modules) override;
        void Unload() override;
};
}
