#pragma once

#include "LayerMask.h"
#include "IClickable.h"
#include "directx/math/DirectXMath.h"

#include <vector>
#include <utility>

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
            void RaycastToIClickables(DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, std::pair<unsigned, unsigned> windowDimensions, LayerMask mask);

        private:
            std::vector<nc::physics::IClickable*> m_clickableComponents;
    };
} //end namespace nc::physics
