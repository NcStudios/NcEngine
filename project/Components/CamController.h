#pragma once
#include "component/Transform.h"
#include "math/Vector2.h"

class CamController : public nc::Component
{
    public:
        CamController(nc::EntityHandle handle);
        void FrameUpdate(float dt) override;

    private:
        nc::Transform * m_mainCameraTransform;

        nc::Vector3 GetCameraZoomMovement();
        nc::Vector3 GetCameraPanMovement();
        void GetCameraRotation();
};