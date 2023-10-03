#include "TimeImpl.h"
#include "task/TaskGraph.h"
#include "time/Time.h"
#include "utility/Log.h"

#include <utility>

#include <iostream>

namespace
{
float g_dt = 0.0f;

auto DeltaTimeInSeconds(const auto& start, const auto& end) -> float
{
    constexpr float microsecondsPerSecond = 1000000.0f;
    const auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return (float)microseconds / microsecondsPerSecond;
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
    NC_LOG_TRACE("Creating Time module");
    return std::make_unique<TimeImpl>();
}

void TimeImpl::OnBuildTaskGraph(task::TaskGraph& graph)
{
    NC_LOG_TRACE("Building Time workload");
    graph.Add
    (
        task::ExecutionPhase::Begin,
        "TimeModule",
        [this]()
        {
            /** @todo shouldn't we do new timepoint first? */
            g_dt = ::DeltaTimeInSeconds(m_lastTime, m_currentTime);
            m_lastTime = std::exchange(m_currentTime, Clock_t::now());
        }
    );
}

void TimeImpl::Clear() noexcept
{
    g_dt = 0.0;
}
} // namespace nc::time
