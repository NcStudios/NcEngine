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
    struct PhysicsSystemInfo
    {
        #ifdef USE_VULKAN
        graphics::Graphics2* graphics;
        #else
        graphics::Graphics* graphics;
        #endif
        job::JobSystem* jobSystem;
        uint32_t maxDynamicColliders;
        uint32_t maxStaticColliders;
        uint32_t octreeDensityThreshold;
        float octreeMinimumExtent;
        float worldspaceExtent;
    };

    class PhysicsSystem
    {
        public:
            PhysicsSystem(const PhysicsSystemInfo& info);

            ecs::ComponentSystem<Collider>* GetColliderSystem();
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
