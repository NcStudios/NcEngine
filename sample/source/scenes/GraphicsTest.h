#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class GraphicsTest : public Scene
{
    public:
        GraphicsTest(SampleUI* ui, Vector3 extents);
        void Load(ecs::Ecs world, ModuleProvider modules) override;
        void Unload() override;

    private:
        SampleUI* m_sampleUI;
};
}
