#pragma once

#include "CollisionSystem.h"
#include "physics/IClickable.h"
#include "directx/math/DirectXMath.h"

#include <vector>

namespace nc::graphics
{
    class FrameManager;
    class Graphics;
    class Graphics2;
}

namespace nc::physics
{
    class PhysicsSystem
    {
        public:
            #ifdef USE_VULKAN
            PhysicsSystem(graphics::Graphics2* graphics2);
            #else
            PhysicsSystem(graphics::Graphics* graphics);
            #endif

            void RegisterClickable(IClickable* toAdd);
            void UnregisterClickable(IClickable* toRemove) noexcept;
            IClickable* RaycastToClickables(LayerMask mask);
            void DoPhysicsStep();
            void ClearState();

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidgets(graphics::FrameManager* frameManager); // hacky solution until widgets are a real thing
            #endif

        private:
            CollisionSystem m_collisionSystem;
            std::vector<nc::physics::IClickable*> m_clickableComponents;
            #ifdef USE_VULKAN
            graphics::Graphics2* m_graphics2;
            #else
            graphics::Graphics* m_graphics;
            #endif
    };
} //end namespace nc::physics
