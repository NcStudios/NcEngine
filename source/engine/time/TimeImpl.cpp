#include "TimeImpl.h"
#include "task/Job.h"
#include "time/Time.h"

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

auto TimeImpl::BuildWorkload() -> std::vector<task::Job>
{
    return std::vector<task::Job>
    {
        task::Job
        {
            [this]()
            {
                /** @todo shouldn't we do new timepoint first? */
                g_dt = ::DeltaTimeInSeconds(m_lastTime, m_currentTime);
                m_lastTime = std::exchange(m_currentTime, Clock_t::now());
            },
            "TimeModule",
            task::ExecutionPhase::Begin
        }
    };
}

void TimeImpl::Clear() noexcept
{
    g_dt = 0.0;
}
} // namespace nc::time