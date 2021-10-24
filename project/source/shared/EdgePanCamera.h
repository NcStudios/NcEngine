#pragma once

#include "ecs/component/Camera.h"

namespace nc::sample
{
    class EdgePanCamera : public Camera
    {
        public:
            EdgePanCamera(Entity entity);
            void FrameUpdate(float dt) override;

        private:
            float m_lastFrameZoom;
            Vector3 m_lastFrameTranslation = Vector3::Zero();
    };
}