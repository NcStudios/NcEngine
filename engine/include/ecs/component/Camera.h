#pragma once

#include "Component.h"
#include "physics/Geometry.h"

#include "directx/Inc/DirectXMath.h"

namespace nc
{
    /** @brief Basic camera component. */
    class Camera : public FreeComponent
    {
        public:
            /**
             * @brief Construct a new Camera object.
             * @param self The parent entity.
             */
            Camera(Entity self) noexcept;

            /**
             * @brief Get the camera's view matrix.
             * @return DirectX::FXMMATRIX
             */
            auto ViewMatrix() const noexcept -> DirectX::FXMMATRIX { return m_view; }

            /**
             * @brief Get the camera's projection matrix.
             * @return DirectX::FXMMATRIX
             */
            auto ProjectionMatrix() const noexcept -> DirectX::FXMMATRIX { return m_projection; }

            /**
             * @brief Calculate the camera's viewport.
             * @return Frustum
             */
            auto CalculateFrustum() const noexcept -> Frustum;

            /** @brief Construct a new view matrix based on the current transform. */
            virtual void UpdateViewMatrix();

            /**
             * @brief Construct a new projection matrix based on input values.
             * @param width Screen width
             * @param height Screen height
             * @param nearZ Viewport near z plane
             * @param farZ Viewport far z plane
             */
            virtual void UpdateProjectionMatrix(float width, float height, float nearZ, float farZ);

            #ifdef NC_EDITOR_ENABLED
            void ComponentGuiElement() override;
            #endif

        private:
            DirectX::XMMATRIX m_view;
            DirectX::XMMATRIX m_projection;
    };
}