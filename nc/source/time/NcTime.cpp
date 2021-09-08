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
        : m_lastTime{Clock::now()},
          m_currentTime{Clock::now()},
          m_frameDeltaTime{0.0},
          m_accumulatedTime{0.0}
    {
    }

    double Time::UpdateTime() noexcept
    {
        const auto cycleDelta = duration_cast<microseconds>(m_currentTime - m_lastTime).count();
        m_frameDeltaTime = cycleDelta / MicrosecondsPerSecond;
        m_accumulatedTime += m_frameDeltaTime;
        m_lastTime = m_currentTime;
        m_currentTime  = Clock::now();
        return m_frameDeltaTime;
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