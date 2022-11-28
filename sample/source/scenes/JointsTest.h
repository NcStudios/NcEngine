#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class JointsTest : public Scene
{
    public:
        JointsTest(SampleUI* ui);
        void Load(Registry* registry, ModuleProvider modules) override;
};
}
