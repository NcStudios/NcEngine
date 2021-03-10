#include "NcTime.h"

namespace
{
    constexpr double MicrosecondsPerSecond = 1000000.0;
}

namespace nc::time
{
    using std::chrono::time_point;
    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    using std::chrono::nanoseconds;

    Time::Time() noexcept
    {
        m_lastTime = Clock::now();
        m_currentTime = Clock::now();
    }

    double Time::GetFixedDeltaTime() const noexcept
    {
        return m_fixedDeltaTime;
    }

    double Time::GetFrameDeltaTime() const noexcept
    {
        return m_frameDeltaTime;
    }

    void Time::ResetFixedDeltaTime() noexcept
    {
        m_fixedDeltaTime = 0.0f;
    }

    void Time::ResetFrameDeltaTime() noexcept
    {
        m_frameDeltaTime = 0.0f;
    }

    void Time::UpdateTime() noexcept
    {
        const auto cycleDelta = duration_cast<microseconds>(m_currentTime - m_lastTime).count();
        m_frameDeltaTime += cycleDelta / MicrosecondsPerSecond;
        m_fixedDeltaTime += cycleDelta / MicrosecondsPerSecond;
        m_lastTime = m_currentTime;
        m_currentTime  = Clock::now();
    }

    void Timer::Start() noexcept
    {
        m_start = Clock::now();
    }

    void Timer::Stop() noexcept
    {
        m_end = Clock::now();
        auto u_sec = duration_cast<nanoseconds>(m_end - m_start).count();
        m_time = u_sec;
    }

    float Timer::Value() const noexcept
    {
        return m_time;
    }
} // namespace nc::time