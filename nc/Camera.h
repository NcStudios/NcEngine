#pragma once
#include "DirectXMath.h"

namespace nc::engine { class RenderingSystem; }

namespace nc
{
    class Camera
    {
        friend nc::engine::RenderingSystem;

        public:
            DirectX::XMMATRIX GetMatrix() const noexcept;

        public:
            float zPos = 15.0f;
            float theta = 0.0f;
            float phi = 0.0f;
            float xRot = 0.0f;
            float yRot = 0.0f;
            float zRot = 0.0f;
    };
}