#pragma once

#include "ncengine/ecs/FrameLogic.h"

namespace nc
{
/** Helper for invoking a FreeComponent's Run method from one of the logic components. */
template<std::derived_from<FreeComponent> T>
struct InvokeFreeComponent
{
    /** The default constructor expects the component to be added externally. */
    InvokeFreeComponent() = default;

    /** The user-defined constructor attaches an instance of the component to be invoked. */
    template<class ... Args>
    InvokeFreeComponent(Entity self, ecs::Ecs world, Args&&... args)
    {
        world.Emplace<T>(self, std::forward<Args>(args)...);
    }

    /** FrameLogic call operator */
    void operator()(Entity self, ecs::Ecs world, float dt) const
    {
        if(world.Contains<T>(self))
            world.Get<T>(self).Run(self, world, dt);
    }
};
} // namespace nc
