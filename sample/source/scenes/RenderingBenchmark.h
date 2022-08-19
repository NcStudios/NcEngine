#pragma once

#include "scene/Scene.h"
#include "shared/SampleUI.h"

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