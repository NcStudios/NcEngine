#pragma once
#include "NCE.h"

using namespace nc;


class CamController : public Component
{
    public:
        CamController(ComponentHandle handle, EntityView parentHandle);

        void OnInitialize() override;
        void FrameUpdate(float dt) override;

    private:
        
        struct CamActionState
        {
            bool    isActive     = false;
            Vector3 initialCam   = {0.0f,0.0f,0.0f};
            Vector2 initialMouse = {0.0f,0.0f};
        } m_camPanState, m_camRotateState;

        Vector3 GetCameraZoomMovement();
        Vector3 GetCameraPanMovement();
        Vector3 GetCameraRotationMovement();

};