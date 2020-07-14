#pragma once
#include "NcCommon.h"
#include "math/Vector2.h"

class CamController : public nc::Component
{
    public:
        CamController();

        void FrameUpdate(float dt) override;

    private:
        
        struct CamActionState
        {
            nc::Vector2 initialMouse = {0.0f,0.0f};
            nc::Vector3 initialCam = {0.0f,0.0f,0.0f};
            bool isActive = false;
        } m_camPanState, m_camRotateState;

        nc::Transform * m_mainCameraTransform = nullptr;

        nc::Vector3 GetCameraZoomMovement();
        nc::Vector3 GetCameraPanMovement();
        nc::Vector3 GetCameraRotationMovement();

};