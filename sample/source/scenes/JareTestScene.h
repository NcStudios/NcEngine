#pragma once

#include "shared/SampleUI.h"

#include "ncengine/scene/Scene.h"

namespace nc::sample
{
class JareTestScene : public Scene
{
    public:
        JareTestScene(SampleUI* ui);
        ~JareTestScene() noexcept;
        void Load(Registry* registry, ModuleProvider modules) override;
        void Unload() override;

    private:
        SampleUI* m_sampleUI;
};
}
