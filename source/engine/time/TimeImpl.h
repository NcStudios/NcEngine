#pragma once

#include "ncengine/module/Module.h"
#include "ncengine/type/EngineId.h"

#include <chrono>
#include <memory>

namespace nc::time
{
/** Time module implementation */
class TimeImpl : public Module
{
    public:
        explicit TimeImpl() noexcept
            : Module{NcTimeId} {}

        void OnBuildTaskGraph(task::TaskGraph&) override;
        void Clear() noexcept override;

    private:
        using Clock_t = std::chrono::high_resolution_clock;
        using TimePoint_t = std::chrono::time_point<Clock_t>;
        TimePoint_t m_lastTime = Clock_t::now();
        TimePoint_t m_currentTime = Clock_t::now();
};

/** Factory for creating an NcTime module */
auto BuildTimeModule() -> std::unique_ptr<TimeImpl>;
} // namespace nc::time
