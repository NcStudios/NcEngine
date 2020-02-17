#ifndef INITIAL_SCENE_H
#define INITIAL_SCENE_H

#include "../../NCE/include/Scene.h"

#include <random>
#include "../../NCE/include/NCE.h"
#include "../../NCE/include/Component.h"
#include "../../NCE/include/Entity.h"
#include "../../NCE/include/Transform.h"
#include "../../NCE/include/Vector.h"

#include "../Components/PointSpawner.h"

class InitialScene : public nc::scene::Scene
{
    public:
        void Load() override;
        void Unload() override;
};


#endif