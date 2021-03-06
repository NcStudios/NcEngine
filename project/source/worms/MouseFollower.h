#pragma once

#include "component/Transform.h"
#include "Window.h"

namespace nc::sample
{
    class MouseFollower : public Component, public window::IOnResizeReceiver
    {
        public:
            MouseFollower(EntityHandle handle);
            ~MouseFollower() noexcept;
            void FrameUpdate(float dt) override;
            void OnResize(Vector2 screenDimensions) override;

        private:
            float m_viewPortDist;
            Vector2 m_screenDimensions;
            Transform* m_transform;
            float m_zDepth;
            float m_zRatio;
    };
}