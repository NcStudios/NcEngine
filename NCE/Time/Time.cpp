#include "Time.hpp"

namespace NCE
{
    uint64_t Time::lastDelta = 0;

    Time::Time()
    {
        m_lastTime = Clock::now();
        m_currentTime = Clock::now();
    }

    void Time::UpdateTime()
    {
        Time::lastDelta = duration_cast<microseconds>(m_currentTime - m_lastTime).count();
        m_lastTime = m_currentTime;
        m_currentTime  = Clock::now();
    }

    uint64_t Time::GetDeltaTime()
    {
        return Time::lastDelta;
    }


}