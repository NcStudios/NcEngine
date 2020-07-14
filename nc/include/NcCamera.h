#pragma once
#include "component/Camera.h"

namespace nc
{
    class Transform;

    /**
     * Sets the camera that NcGetMainCameraTransform()
     * refers to.
     */
    void NcRegisterMainCamera(Camera * camera);

    /** 
     * Get pointer to the main camera.
     * @return Pointer to main camera's transform, nullptr 
     * if no camera has been registered.
     */
    Transform * NcGetMainCameraTransform();
}