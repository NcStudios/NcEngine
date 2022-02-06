#pragma once

#include "Registry.h"
#include "component/Component.h"

namespace nc
{
    /** @todo what does it look like if we overload operator() to handle invoking for
     *  1. different logic types: frame/fixed/collision
     *  2. is there a way to overload for the same logic type: Frame(Entity) and Frame(Entity, Reg*, float)
    */

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