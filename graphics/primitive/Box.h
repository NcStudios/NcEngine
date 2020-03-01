#pragma once
#include "../internal/DrawableBase.h"
#include <random>

namespace nc::graphics::primitive
{
    class Box : public nc::graphics::internal::DrawableBase<Box>
    {
        public:
            Box(nc::graphics::internal::Graphics& graphics, std::mt19937& rng,
                 std::uniform_real_distribution<float>& adist,
                 std::uniform_real_distribution<float>& ddist,
                 std::uniform_real_distribution<float>& odist,
                 std::uniform_real_distribution<float>& rdist,
                 DirectX::XMFLOAT3 materialColor);

            void Update(float dt) noexcept override;
            DirectX::XMMATRIX GetTransformXM() const noexcept override;
            
        private:
            //DirectX::XMFLOAT3 m_materialColor;
            
            // positional
            float r;
            float roll = 0.0f;
            float pitch = 0.0f;
            float yaw = 0.0f;
            float theta;
            float phi;
            float chi;
            // speed (delta/s)
            float droll;
            float dpitch;
            float dyaw;
            float dtheta;
            float dphi;
            float dchi;
        };
}