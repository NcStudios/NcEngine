#pragma once
#include "component/Transform.h"
#include "NcConfig.h"
#include "math/Vector2.h"

class CamController : public nc::Component
{
    public:
        CamController();
        void FrameUpdate(float dt) override;

    private:
        nc::Transform * m_mainCameraTransform = nullptr;
        const nc::config::Config& m_config;

        nc::Vector3 GetCameraZoomMovement();
        nc::Vector3 GetCameraPanMovement();
};