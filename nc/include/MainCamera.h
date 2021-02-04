#pragma once

#include "component/Camera.h"
#include "component/Transform.h"

namespace nc::camera
{
    /** Specifies the camera to use for view and projection calculations.
     *  A valid camera must be set during frame execution. */
    void SetMainCamera(Camera* camera);

    /** Get the transform of the current main camera */
    [[nodiscard]] Transform* GetMainCameraTransform();
}