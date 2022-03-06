#pragma once

#include "ecs/Registry.h"
#include "math/Random.h"
#include "scene/SceneSystemImpl.h"
#include "time/Time.h"

namespace nc
{
    struct Context
    {
        Registry registry;
        time::Time time;
        scene::SceneSystemImpl scene;
        nc::Random random;
    };
}