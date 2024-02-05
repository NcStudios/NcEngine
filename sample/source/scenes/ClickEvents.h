#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class ClickEvents : public Scene
{
    public:
        ClickEvents(SampleUI* ui);
        ~ClickEvents() noexcept;
        void Load(Registry* registry, ModuleProvider modules) override;

    private:
        SampleUI* m_sampleUI;
};
}
