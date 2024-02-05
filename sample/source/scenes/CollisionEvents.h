#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class CollisionEvents : public Scene
{
    public:
        CollisionEvents(SampleUI* ui);
        ~CollisionEvents() noexcept;
        void Load(Registry* registry, ModuleProvider modules) override;

    private:
        SampleUI* m_sampleUI;
};
}
