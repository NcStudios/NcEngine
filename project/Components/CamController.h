#pragma once
#include "NCE.h"

using namespace nc;


class CamController : public Component
{
    public:
        CamController();

        void FrameUpdate(float dt) override;

    private:
        
        struct CamActionState
        {
            bool    isActive     = false;
            Vector3 initialCam   = {0.0f,0.0f,0.0f};
            Vector2 initialMouse = {0.0f,0.0f};
        } m_camPanState, m_camRotateState;

        Transform * m_mainCameraTransform = nullptr;

        Vector3 GetCameraZoomMovement();
        Vector3 GetCameraPanMovement();
        Vector3 GetCameraRotationMovement();

};