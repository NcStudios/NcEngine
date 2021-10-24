#pragma once

#include "Component.h"
#include "physics/Geometry.h"

#include "directx/math/DirectXMath.h"

namespace nc
{
    class Camera : public AutoComponent
    {
        public:
            Camera(Entity entity) noexcept;

            auto GetViewMatrix() const noexcept -> DirectX::FXMMATRIX { return m_view; }
            auto GetProjectionMatrix() const noexcept -> DirectX::FXMMATRIX { return m_projection; }
            auto CalculateFrustum() const noexcept -> Frustum;

            void UpdateViewMatrix();
            void UpdateProjectionMatrix(float width, float height, float nearZ, float farZ);

            #ifdef NC_EDITOR_ENABLED
            void ComponentGuiElement() override;
            #endif
        
        private:
            DirectX::XMMATRIX m_view;
            DirectX::XMMATRIX m_projection;
    };
}