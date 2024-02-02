#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class RenderingBenchmark : public Scene
{
    public:
        RenderingBenchmark(SampleUI* ui);
        ~RenderingBenchmark() noexcept;
        void Load(Registry* registry, ModuleProvider modules) override;
        void Unload() override;

    private:
        SampleUI* m_sampleUI;
};
}
