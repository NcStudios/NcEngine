#pragma once

#include "Ecs.h"

namespace nc::sample
{
    /** Destroys entities on collision if they do not also have this component. */ 
    class DestroyOtherOnCollision : public Component
    {
        public:
            DestroyOtherOnCollision(EntityHandle handle);
            void OnCollisionEnter(Entity* other) override;
    };

    inline DestroyOtherOnCollision::DestroyOtherOnCollision(EntityHandle handle)
        : Component{handle}
    {
    }

    inline void DestroyOtherOnCollision::OnCollisionEnter(Entity* other)
    {
        if(other && !other->HasUserComponent<DestroyOtherOnCollision>())
            DestroyEntity(other->Handle);
    }
}