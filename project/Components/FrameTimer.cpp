#include "FrameTimer.h"
#include "Input.h"
#include "source/log/GameLog.h"

#include <chrono>
#include <iostream>

using namespace nc;
using namespace nc::input;

namespace project
{
    FrameTimer::FrameTimer(EntityHandle handle)
        : Component(handle)
    {
    }

    void FrameTimer::FrameUpdate(float dt)
    {
        if (m_started)
        {
            ++m_frames;
            m_seconds += dt;
        }

        if (GetKeyDown(KeyCode::Space))
        {
            m_started ? stop() : start();
        }
    }

    void FrameTimer::start()
    {
        log::GameLog::Log("--Starting Timer--");
        //std::cout << "\n--Starting Timer--\n";
        m_frames  = 0;
        m_seconds = 0.0f;
        m_started = true;
    }

    void FrameTimer::stop()
    {
        std::cout << "--Timer Results--" << '\n'
                << "-  Frames:  " << m_frames << '\n'
                << "-  Seconds: " << m_seconds << '\n'
                << "-  FPS:     " << (float)m_frames / m_seconds << '\n'
                << "--Stopping Timer--\n";
        m_started = false;
    }
}