#pragma once
#include "nc/source/component/Transform.h"
#include "Engine.h"
#include "nc/source/math/Vector2.h"

class CamController : public nc::Component
{
    public:
        CamController(nc::EntityHandle handle);
        void FrameUpdate(float dt) override;

    private:
        nc::Transform * m_mainCameraTransform;
        const nc::config::Config& m_config;

        nc::Vector3 GetCameraZoomMovement();
        nc::Vector3 GetCameraPanMovement();
};