#pragma once

#include "ecs/component/Component.h"
#include "Window.h"

namespace nc { class Registry; }

namespace nc::sample
{
    class MouseFollower : public AutoComponent, public window::IOnResizeReceiver
    {
        public:
            MouseFollower(Entity entity, Registry* registry);
            ~MouseFollower() noexcept;
            void FrameUpdate(float dt) override;
            void OnResize(Vector2 screenDimensions) override;

        private:
            Registry* m_registry;
            float m_viewPortDist;
            Vector2 m_screenDimensions;
            float m_zDepth;
            float m_zRatio;
    };
}