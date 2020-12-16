#pragma once
#include "nc/source/component/Transform.h"
#include "window/IOnResizeReceiver.h"

namespace project
{
    class MouseFollower : public nc::Component, public nc::window::IOnResizeReceiver
    {
        public:
            MouseFollower();
            ~MouseFollower();
            void FrameUpdate(float dt) override;
            void OnResize(nc::Vector2 screenDimensions) override;

        private:

            float m_viewPortDist;
            nc::Vector2 m_screenDimensions;
            nc::Transform* m_transform;
            float m_zDepth;
            float m_zRatio;
    };
}