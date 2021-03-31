#pragma once

#include "CollisionSystem.h"
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

            void DoPhysicsStep();
            void ClearState();

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidgets(graphics::FrameManager* frameManager); // hacky solution until widgets are a real thing
            #endif

        private:
            CollisionSystem m_collisionSystem;
            ClickableSystem m_clickableSystem;
    };
} // namespace nc::physics
