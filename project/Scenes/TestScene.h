#pragma once

#include <random>
#include "scene/Scene.h"


class TestScene : public nc::scene::Scene
{
    public:
        void Load() override;
        void Unload() override;
};