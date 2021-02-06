#pragma once

#include "component/Component.h"

namespace project
{
    class FrameTimer : public nc::Component
    {
        public:
            FrameTimer(nc::EntityHandle handle);
            void FrameUpdate(float dt) override;

        private:
            int m_frames = 0;
            float m_seconds = 0.0f; 
            bool m_started = false;

            void start();
            void stop();
    };
}