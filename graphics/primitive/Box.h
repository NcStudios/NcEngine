#pragma once
#include "../internal/DrawableBase.h"
#include <random>

namespace nc::graphics::internal
{
    class Graphics;
}

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

            void SpawnControlWindow(int id, nc::graphics::internal::Graphics& graphics);
            
        private:
            void SyncMaterialData(nc::graphics::internal::Graphics& graphics);

            //DirectX::XMFLOAT3 m_materialColor;
            struct PSMaterialConstants
            {
                DirectX::XMFLOAT3 color;
                float specularIntensity = 0.6;
                float specularPower = 30.0f;
                float padding[3];
            } m_materialData;

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