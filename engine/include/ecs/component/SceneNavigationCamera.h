#pragma once

#include "Camera.h"

namespace nc
{
    class Registry;

    /** @brief Movement settings for SceneNavigationCamera. */
    struct SceneCameraConfig
    {
        float panDamp = 0.25f;
        float lookDamp = 0.005f;
        float zoomDamp = 1.4f;
        float zoomSpeedMult = 8.0f;
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
            Vector2 m_panStartPos = Vector2::Zero();
            Vector2 m_lookStartPos = Vector2::Zero();
            float m_panUnitsTraveled = 0.0f;
            float m_zoomVelocity = 0.0f;

            auto HandlePanInput() -> bool;
            auto HandleZoomInput() -> bool;
            auto HandleLookInput() -> bool;
            auto Pan(float dt) -> Vector3;
            auto Zoom(float dt) -> Vector3;
            auto Look(float dt, const Vector3& transformRight) -> Quaternion;
    };
} // end namespace nc