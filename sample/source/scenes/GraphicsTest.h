#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class GraphicsTest : public Scene
{
    public:
        static const nc::Vector3 Extents;
        GraphicsTest(SampleUI* ui, Vector3 extents);
        void Load(ecs::Ecs world, ModuleProvider modules) override;
        void Unload() override;

    private:
        SampleUI* m_sampleUI;
};
}
