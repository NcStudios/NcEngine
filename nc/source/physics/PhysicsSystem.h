#pragma once

#include "collision/CollisionSystem.h"
#include "ClickableSystem.h"

namespace nc
{
    namespace graphics { class FrameManager; }
    namespace job { class JobSystem; }
}

namespace nc::physics
{
    class PhysicsSystem
    {
        public:
            #ifdef USE_VULKAN
            PhysicsSystem(graphics::Graphics2* graphics, ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem);
            #else
            PhysicsSystem(graphics::Graphics* graphics, ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem);
            #endif

            void DoPhysicsStep(float dt);
            void ClearState();

        private:
            CollisionSystem m_collisionSystem;
            ClickableSystem m_clickableSystem;
    };
} // namespace nc::physics
