#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class RenderingBenchmark : public Scene
{
    public:
        RenderingBenchmark(SampleUI* ui);
        void Load(Registry* registry, ModuleProvider modules) override;
        void Unload() override;
};
}
