#ifndef INITIAL_SCENE_H
#define INITIAL_SCENE_H

#include <random>
#include "Scene.h"
#include "NCE.h"
#include "Head.h"

class InitialScene : public nc::scene::Scene
{
    public:
        void Load() override;
        void Unload() override;
};


#endif