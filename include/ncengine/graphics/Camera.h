#pragma once

#include "ncengine/ecs/Component.h"

#include "ncmath/Geometry.h"

#include "DirectXMath.h"

namespace nc::graphics
{
/** @brief Properties for controlling a camera's frustum. */
struct CameraProperties
{
    /** @brief Camera field of view in radians. */
    float fov = 1.0472f;

    /** @brief Distance to near clipping plane. Must be greater than 0. */
    float nearClip = 0.1f;

    /** @brief Distance to far clipping plane. Must be greater than nearClip. */
    float farClip = 400.0f;
};

/** @brief Two points lying on a Camera's near and far planes. */
struct NearFarPoints
{
    Vector3 nearPoint = Vector3::Zero();
    Vector3 farPoint = Vector3::Zero();
};

/** @brief Basic camera component. */
class Camera : public FreeComponent
{
    public:
        /**
         * @brief Construct a new Camera object.
         * @param self The parent entity.
         * @param properties Camera properties.
         */
        Camera(Entity self, const CameraProperties& properties = {}) noexcept;

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

        /** @brief Unproject 2D normalized device coordinates into 3D points on the near and far planes. */
        auto CastToNearAndFarPlanes(const Vector2& normalizedDeviceCoords) const -> NearFarPoints;

        /**
         * @brief Calculate the camera's viewport.
         * @return Frustum
         */
        auto CalculateFrustum() const noexcept -> Frustum;

        /** @brief Construct a new view matrix based on the current transform. */
        virtual void UpdateViewMatrix(DirectX::FXMMATRIX transformationMatrix);

        /**
         * @brief Construct a new projection matrix based on input values.
         * @param width Screen width
         * @param height Screen height
         */
        virtual void UpdateProjectionMatrix(float width, float height);

        /**
         * @brief Enable execution of the Run() method in thhe derived class.
         * @note It is only necessary to implement this if the derived class has update logic running in a FrameLogic.
         */
        virtual void EnableUpdate() noexcept {}

        /**
         * @brief Disable execution of the Run() method in the derived class.
         * @note It is only necessary to implement this if the derived class has update logic running in a FrameLogic.
         */
        virtual void DisableUpdate() noexcept {}

        #ifdef NC_EDITOR_ENABLED
        void ComponentGuiElement() override;
        #endif

    private:
        DirectX::XMMATRIX m_view;
        DirectX::XMMATRIX m_projection;
        CameraProperties m_properties;
};
} // namespace nc::graphics
