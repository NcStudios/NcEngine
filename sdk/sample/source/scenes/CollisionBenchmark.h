#pragma once
#include "scene/Scene.h"
#include "shared/SampleUI.h"

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