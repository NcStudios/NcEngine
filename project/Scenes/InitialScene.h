#pragma once

#include <random>
#include "scene/Scene.h"
#include "NCE.h"
#include "Head.h"
#include "CamController.h"

class InitialScene : public nc::scene::Scene
{
    public:
        void Load() override;
        void Unload() override;
};