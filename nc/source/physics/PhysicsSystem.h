#pragma once

#include "ClickableSystem.h"
#include "collision/CollisionCache.h"
#include "dynamics/Joint.h"
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

            void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias, float softness);
            void RemoveJoint(Entity entityA, Entity entityB);
            void RemoveAllJoints(Entity entity);

            void DoPhysicsStep(float dt);
            void ClearState();

            #ifdef NC_DEBUG_RENDERING
            void DebugRender();
            #endif

        private:
            CollisionCache m_cache;
            std::vector<Joint> m_joints;
            ClickableSystem m_clickableSystem;
            job::JobSystem* m_jobSystem;
            #ifdef NC_DEBUG_RENDERING
            graphics::DebugRenderer m_debugRenderer;
            #endif
    };
} // namespace nc::physics
