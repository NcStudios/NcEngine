#pragma once

#include "scene/Scene.h"
#include "NCE.h"

class InitialScene : public nc::scene::Scene
{
    public:
        void Load() override;
        void Unload() override;
};