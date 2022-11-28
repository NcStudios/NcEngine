#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class CollisionEvents : public Scene
{
    public:
        CollisionEvents(SampleUI* ui);
        void Load(Registry* registry, ModuleProvider modules) override;
};
}
