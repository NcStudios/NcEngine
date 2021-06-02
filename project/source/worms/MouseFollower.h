#pragma once

#include "Ecs.h"
#include "Window.h"

namespace nc::sample
{
    class MouseFollower : public AutoComponent, public window::IOnResizeReceiver
    {
        public:
            MouseFollower(Entity entity, registry_type* registry);
            ~MouseFollower() noexcept;
            void FrameUpdate(float dt) override;
            void OnResize(Vector2 screenDimensions) override;

        private:
            registry_type* m_registry;
            float m_viewPortDist;
            Vector2 m_screenDimensions;
            float m_zDepth;
            float m_zRatio;
    };
}