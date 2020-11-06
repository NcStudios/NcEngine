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
            void RegisterClickable(IClickable* toAdd);
            void UnregisterClickable(IClickable* toRemove);
            IClickable* RaycastToClickables(DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, Vector2 windowDimensions, LayerMask mask);

        private:
            std::vector<nc::physics::IClickable*> m_clickableComponents;
    };
} //end namespace nc::physics
