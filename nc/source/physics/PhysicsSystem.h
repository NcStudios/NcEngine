#pragma once

#include "ClickableSystem.h"
#include "collision/CollisionCache.h"
#include "graphics/DebugRenderer.h"

namespace nc
{
    namespace ecs { class ColliderSystem; }
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
            ClickableSystem m_clickableSystem;
            CollisionCache m_cache;
            ecs::ColliderSystem* m_colliderSystem;
            job::JobSystem* m_jobSystem;
            #ifdef NC_DEBUG_RENDERING
            graphics::DebugRenderer m_debugRenderer;
            #endif
    };
} // namespace nc::physics
