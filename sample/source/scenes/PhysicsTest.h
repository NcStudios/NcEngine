#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class PhysicsTest : public Scene
{
    public:
        PhysicsTest(SampleUI* ui);
        void Load(ecs::Ecs world, ModuleProvider modules) override;
        void Unload() override;

    private:
        SampleUI* m_sampleUI;
};
}
