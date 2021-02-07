#pragma once

#include "component/Component.h"
#include "Input.h"
#include "GameLog.h"

#include <chrono>
#include <sstream>

namespace nc::sample
{
    class FrameTimer : public Component
    {
        public:
            FrameTimer(EntityHandle handle);
            void FrameUpdate(float dt) override;

        private:
            int m_frames = 0;
            float m_seconds = 0.0f; 
            bool m_started = false;

            void Start();
            void Stop();
    };

    inline FrameTimer::FrameTimer(EntityHandle handle)
        : Component(handle)
    {
    }

    inline void FrameTimer::FrameUpdate(float dt)
    {
        if (m_started)
        {
            ++m_frames;
            m_seconds += dt;
        }

        if (input::GetKeyDown(input::KeyCode::Space))
            m_started ? Stop() : Start();
    }

    inline void FrameTimer::Start()
    {
        GameLog::Log("--Starting Timer--");
        m_frames  = 0;
        m_seconds = 0.0f;
        m_started = true;
    }

    inline void FrameTimer::Stop()
    {
        std::stringstream s1;
        s1 << "--Stopping Timer--\n"
           << "  Frames:  " << m_frames  << '\n'
           << "  Seconds: " << m_seconds << '\n'
           << "  FPS:     " << (float)m_frames / m_seconds;
        GameLog::Log(s1.str());
        m_started = false;
    }
}