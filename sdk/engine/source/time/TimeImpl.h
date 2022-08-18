#pragma once

#include "module/Module.h"

#include <chrono>

namespace nc::time
{
/** Time module implementation */
class TimeImpl : public Module
{
    public:
        auto BuildWorkload() -> std::vector<Job> override;
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
