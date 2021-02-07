#pragma once

#include "component/Transform.h"
#include "math/Vector2.h"

namespace nc::sample
{
    class EdgePanCamera : public Component
    {
        public:
            EdgePanCamera(EntityHandle handle);
            void FrameUpdate(float dt) override;

        private:
            Transform* m_mainCameraTransform;
            float m_lastFrameZoom;
            Vector3 m_lastFrameTranslation = Vector3::Zero();
    };
}