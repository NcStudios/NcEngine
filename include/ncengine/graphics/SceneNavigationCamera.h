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
    float truckPedestalFine = 0.8f;
    float truckPedestalCoarse = 3.0f;
    float panTiltFine = 0.006f;
    float panTiltCoarse = 0.018f;
    float dollyFine = 0.0025f;
    float dollyCoarse = 0.025f;
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
        SceneNavigationCamera(Entity self,
                              const CameraProperties& cameraProperties = {},
                              const SceneCameraConfig& config = {});

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
        Vector2 m_prevMousePos = Vector2::Zero();
        Vector2 m_pivotReference = Vector2::Zero();
        float m_dollyVelocity = 0.0f;

        auto HandlePanInput() -> bool;
        auto HandleZoomInput() -> bool;
        auto HandleLookInput() -> bool;
        auto TruckAndPedestal(float dt, float speedMult) -> Vector3;
        auto PanAndTilt(float dt, float speedMult, const Vector3& tiltAxis) -> Quaternion;
        auto Dolly(float dt, float speedMult) -> Vector3;
};
} // namespace graphics
} // namespace nc
