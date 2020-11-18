#pragma once
#include "component/Component.h"
#include "component/Transform.h"

namespace project
{
    class MouseFollower : public nc::Component
    {
        public:
            MouseFollower();
            void FrameUpdate(float dt) override;

        private:

            float m_viewPortDist;
            nc::Vector2 m_screenDimensions;
            nc::Transform* m_transform;
    };
}