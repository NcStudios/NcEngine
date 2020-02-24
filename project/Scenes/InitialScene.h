#ifndef INITIAL_SCENE_H
#define INITIAL_SCENE_H

#include <random>
#include "../../NCE/include/Scene.h"
#include "../../NCE/include/NCE.h"
#include "../Components/Head.h"
#include "../Components/PointSpawner.h"

class InitialScene : public nc::scene::Scene
{
    public:
        void Load() override;
        void Unload() override;
};


#endif