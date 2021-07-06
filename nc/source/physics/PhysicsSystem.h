#pragma once

#include "collision/CollisionSystem.h"
#include "ClickableSystem.h"
#include "graphics/DebugRenderer.h"

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

            #ifdef NC_DEBUG_RENDERING
            void DebugRender();
            #endif

        private:
            CollisionSystem m_collisionSystem;
            ClickableSystem m_clickableSystem;
            #ifdef NC_DEBUG_RENDERING
            graphics::DebugRenderer m_debugRenderer;
            #endif
    };
} // namespace nc::physics
