#pragma once

#include "physics/LayerMask.h"
#include "physics/IClickable.h"

#include <functional>

namespace nc::engine { class ApiBinder; }

namespace nc::physics
{
    class Physics
    {
        public:
            static void RegisterClickable(IClickable* clickable);
            static void UnregisterClickable(IClickable* clickable);
            static IClickable* RaycastToClickables(LayerMask mask = LAYER_MASK_ALL);

        private:
            friend nc::engine::ApiBinder;
            static std::function<void(IClickable*)> RegisterClickable_;
            static std::function<void(IClickable*)> UnregisterClickable_;
            static std::function<IClickable*(LayerMask)> RaycastToClickables_;
    };
}