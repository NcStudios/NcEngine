#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class Benchmarks : public Scene
{
    public:
        Benchmarks(SampleUI* ui);
        void Load(ecs::Ecs world, ModuleProvider modules) override;
        void Unload() override;

    private:
        SampleUI* m_sampleUI;
};
} // namespace nc::sample
