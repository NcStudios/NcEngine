#pragma once

#include "ClickableSystem.h"
#include "collision/CollisionCache.h"
#include "assets/HullColliderManager.h"
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
            PhysicsSystem(graphics::Graphics2* graphics, job::JobSystem* jobSystem);
            #else
            PhysicsSystem(graphics::Graphics* graphics, job::JobSystem* jobSystem);
            #endif

            void DoPhysicsStep(float dt);
            void ClearState();

            #ifdef NC_DEBUG_RENDERING
            void DebugRender();
            #endif

        private:
            CollisionCache m_cache;
            ClickableSystem m_clickableSystem;
            HullColliderManager m_hullColliderManager;
            job::JobSystem* m_jobSystem;
            #ifdef NC_DEBUG_RENDERING
            graphics::DebugRenderer m_debugRenderer;
            #endif
    };
} // namespace nc::physics
