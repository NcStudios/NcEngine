#pragma once

#include "component/Collider.h"
#include "physics/IClickable.h"
#include "directx/math/DirectXMath.h"

#include <vector>

namespace nc
{
    class Transform;
    namespace graphics { class Graphics; class Graphics2; }
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
            void UnregisterClickable(IClickable* toRemove);
            IClickable* RaycastToClickables(LayerMask mask);
            void DoPhysicsStep(const std::vector<Collider*>& colliders);
            void ClearState();

        private:
            std::vector<nc::physics::IClickable*> m_clickableComponents;
            #ifdef USE_VULKAN
            graphics::Graphics2* m_graphics2;
            #else
            graphics::Graphics* m_graphics;
            #endif
    };
} //end namespace nc::physics
