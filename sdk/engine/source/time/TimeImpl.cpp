#include "time/Time.h"
#include "time/TimeImpl.h"
#include "module/Job.h"

namespace
{
float g_dt = 0.0f;

auto DeltaTimeInSeconds(const auto& start, const auto& end) -> float
{
    /** @todo Was there any reason to not use nanoseconds here? */
    constexpr float microsecondsPerSecond = 1000000.0f;
    const auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return microseconds / microsecondsPerSecond;
}
}

namespace nc::time
{
/** From time/Time.h */
auto DeltaTime() noexcept -> float
{
    return g_dt;
}

auto BuildTimeModule() -> std::unique_ptr<TimeImpl>
{
    return std::make_unique<TimeImpl>();
}

auto TimeImpl::BuildWorkload() -> std::vector<Job>
{
    return std::vector<Job>
    {
        Job
        {
            [this]()
            {
                /** @todo shouldn't we do new timepoint first? */
                g_dt = ::DeltaTimeInSeconds(m_lastTime, m_currentTime);
                m_lastTime = std::exchange(m_currentTime, Clock_t::now());
            },
            "TimeModule",
            HookPoint::Begin
        }
    };
}

void TimeImpl::Clear() noexcept
{
    g_dt = 0.0;
}
} // namespace nc::time