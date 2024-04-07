#pragma once

#include "ncengine/scene/Scene.h"

#include <functional>

namespace nc::sample
{
class SmokeTest : public Scene
{
    public:
        SmokeTest(std::function<void()> quitEngineCallback);

        void Load(ecs::Ecs world, ModuleProvider modules) override;

    private:
        std::function<void()> m_quitEngine;
};
} // nc::sample
