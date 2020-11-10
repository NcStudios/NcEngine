#pragma once

#include "LayerMask.h"
#include "IClickable.h"
#include "math/Vector2.h"
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
            ~PhysicsSystem();

            static void RegisterClickable(IClickable* toAdd);
            static void UnregisterClickable(IClickable* toRemove);
            static IClickable* RaycastToClickables(LayerMask mask);

            void RegisterClickable_(IClickable* toAdd);
            void UnregisterClickable_(IClickable* toRemove);
            IClickable* RaycastToClickables_(LayerMask mask);

        private:
            static PhysicsSystem* m_instance;
            std::vector<nc::physics::IClickable*> m_clickableComponents;
            graphics::Graphics* m_graphics;
    };
} //end namespace nc::physics
