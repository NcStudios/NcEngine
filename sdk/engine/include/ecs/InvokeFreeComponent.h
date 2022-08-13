#pragma once

#include "Registry.h"

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
    InvokeFreeComponent(Entity self, Registry* registry, Args&&... args)
    {
        registry->Add<T>(self, std::forward<Args>(args)...);
    }

    /** FrameLogic call operator */
    void operator()(Entity self, Registry* registry, float dt) const
    {
        if(auto* component = registry->Get<T>(self))
            component->Run(self, registry, dt);
    }

    /** FixedLogic call operator */
    void operator()(Entity self, Registry* registry) const
    {
        if(auto* component = registry->Get<T>(self))
            component->Run(self, registry);
    }

    /** CollisionLogic call operator */
    void operator()(Entity self, Entity hit, Registry* registry) const
    {
        if(auto* component = registry->Get<T>(self))
            component->Run(self, hit, registry);
    }
};
} // namespace nc