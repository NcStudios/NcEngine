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
            PhysicsSystem();
            ~PhysicsSystem();

            static void RegisterClickable(IClickable* toAdd);
            static void UnregisterClickable(IClickable* toRemove);
            static IClickable* RaycastToClickables(DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, Vector2 windowDimensions, LayerMask mask);

            void RegisterClickable_(IClickable* toAdd);
            void UnregisterClickable_(IClickable* toRemove);
            IClickable* RaycastToClickables_(DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectMatrix, Vector2 windowDimensions, LayerMask mask);

        private:
            static PhysicsSystem* m_instance;
            std::vector<nc::physics::IClickable*> m_clickableComponents;
    };
} //end namespace nc::physics
