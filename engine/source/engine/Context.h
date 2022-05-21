#pragma once

#include "assets/Assets.h"
#include "ecs/Registry.h"
#include "math/Random.h"
#include "time/Time.h"

namespace nc
{
    struct Context
    {
        Registry registry;
        time::Time time;
        nc::Random random;
        // nc::Assets assets;
    };
}