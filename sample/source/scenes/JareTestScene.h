#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class JareTestScene : public Scene
{
    public:
        JareTestScene(SampleUI* ui);
        void Load(Registry* registry, ModuleProvider modules) override;
};
}
