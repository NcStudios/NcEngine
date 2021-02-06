#pragma once
#include "component/Transform.h"
#include "math/Vector2.h"

class EdgePanCamera : public nc::Component
{
    public:
        EdgePanCamera(nc::EntityHandle handle);
        void FrameUpdate(float dt) override;

    private:
        nc::Transform * m_mainCameraTransform;
        float m_lastFrameZoom;
        nc::Vector3 m_lastFrameTranslation = nc::Vector3::Zero();
};