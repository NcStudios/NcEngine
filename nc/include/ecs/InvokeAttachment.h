#pragma once

#include "Registry.h"

namespace nc
{
    /** @todo It may be useful to allow invoking fixed and collision logic as well. */

   /** Helper for invoking a StateAttachment's Run method from a FrameLogic
    *  component. The user-defined constructor can be used to add an instance
    *  of T to the registry. */
    template<std::derived_from<StateAttachment> T>
    struct InvokeAttachment
    {
        InvokeAttachment() = default;

        template<class ... Args>
        InvokeAttachment(Entity self, Registry* registry, Args&&... args)
        {
            registry->Add<T>(self, std::forward<Args>(args)...);
        }

        void operator()(Entity self, Registry* registry, float dt)
        {
            if(auto* attachment = registry->Get<T>(self))
                attachment->Run(self, registry, dt);
        }
    };
}