#pragma once

#include "component/Transform.h"
#include "component/Camera.h"
#include "math/Vector2.h"

namespace nc::sample
{
    class EdgePanCamera : public Camera
    {
        public:
            EdgePanCamera(EntityHandle handle);
            void FrameUpdate(float dt) override;

        private:
            float m_lastFrameZoom;
            Vector3 m_lastFrameTranslation = Vector3::Zero();
    };
}