#pragma once

#include "Camera.h"

namespace nc
{
class Registry;

namespace graphics
{
/** @brief Movement settings for SceneNavigationCamera. */
struct SceneCameraConfig
{
    float truckPedestalFine = 1.8f;
    float truckPedestalCoarse = 7.0f;
    float panTiltFine = 0.004f;
    float panTiltCoarse = 0.008f;
    float dollyFine = 1.2f;
    float dollyCoarse = 2.6f;
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
        Vector3 m_fineSpeed;
        Vector3 m_coarseSpeed;
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
} // end namespace graphics
} // end namespacenc