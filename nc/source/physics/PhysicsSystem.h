#pragma once

#include "component/Collider.h"
#include "physics/IClickable.h"
#include "directx/math/DirectXMath.h"

#include <vector>

namespace nc
{
    class Transform;
    namespace graphics { class Graphics; }
}

namespace nc::physics
{
    class PhysicsSystem
    {
        public:
            PhysicsSystem(graphics::Graphics* graphics);

            void ClearState();

            void DoPhysicsStep(const std::vector<Collider*>& colliders);

            void RegisterClickable(IClickable* toAdd);
            void UnregisterClickable(IClickable* toRemove);
            IClickable* RaycastToClickables(LayerMask mask);

        private:
            std::vector<nc::physics::IClickable*> m_clickableComponents;
            graphics::Graphics* m_graphics;
    };
} //end namespace nc::physics
