#include "NcTime.h"

namespace nc::time{

    uint64_t Time::CycleDeltaTime  = 0;
    double   Time::FixedDeltaTime  = 0;
    double   Time::FrameDeltaTime  = 0;

    Time::Time()
    {
        m_lastTime = Clock::now();
        m_currentTime = Clock::now();
    }

    void Time::UpdateTime()
    {
        Time::CycleDeltaTime = duration_cast<microseconds>(m_currentTime - m_lastTime).count();
        Time::FrameDeltaTime += Time::CycleDeltaTime / 1000000.0;
        Time::FixedDeltaTime += Time::CycleDeltaTime / 1000000.0;
        m_lastTime = m_currentTime;
        m_currentTime  = Clock::now();
    }

    void Time::ResetFixedDeltaTime()
    {
        Time::FixedDeltaTime = 0;
    }

    void Time::ResetFrameDeltaTime()
    {
        Time::FrameDeltaTime = 0;
    }


    void Timer::Start()
    {
        m_start = Clock::now();
    }

    void Timer::Stop()
    {
        m_end = Clock::now();

        auto u_sec = duration_cast<std::chrono::nanoseconds>(m_end - m_start).count();

        m_time = u_sec;// / 1000000.0f;
    }

    float Timer::Value() const
    {
        return m_time;
    }

} //end namespace nc::time