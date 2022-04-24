#pragma once

#include "Camera.h"

namespace nc
{
    class Registry;

    /** @brief Movement settings for SceneNavigationCamera. */
    struct SceneCameraConfig
    {
        Vector3 fineSpeed   = Vector3{1.800f, 0.004f, 1.200f}; // Truck/Pedestal, Pan/Tilt, Dolly
        Vector3 coarseSpeed = Vector3{7.000f, 0.008f, 2.600f};
        float dollyDeceleration = 0.01f;
    };

    /** @brief Camera for the editor's scene view.
     * 
     * Pan:  Hold middle mouse button
     * Look: Hold right mouse button
     * Zoom: Mouse wheel
     */
    class SceneNavigationCamera : public Camera
    {
        public:
            /**
             * @brief Construct a new Scene Navigation Camera object
             * @param self The owning entity
             * @param config Camera movement settings
             */
            SceneNavigationCamera(Entity self, const SceneCameraConfig& config = SceneCameraConfig{});

            /**
             * @brief Handle input and update transform. (for use with FrameLogic/InvokeFreeComponent)
             * @param self The owning entity
             * @param registry Pointer to the active registry
             * @param dt Seconds since last update
             */
            void Run(Entity self, Registry* registry, float dt);

        private:
            SceneCameraConfig m_config;
            Vector2 m_slideReference = Vector2::Zero();
            Vector2 m_pivotReference = Vector2::Zero();
            float m_unitsTraveled = 0.0f;
            float m_dollyVelocity = 0.0f;

            auto HandlePanInput() -> bool;
            auto HandleZoomInput() -> bool;
            auto HandleLookInput() -> bool;
            auto TruckAndPedestal(float dt, float speedMult) -> Vector3;
            auto PanAndTilt(float dt, float speedMult, const Vector3& tiltAxis) -> Quaternion;
            auto Dolly(float dt, float speedMult) -> Vector3;
    };
} // end namespace nc