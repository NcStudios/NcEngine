#pragma once

#include <chrono>
#include <utility>

#include <iostream>

namespace nc::time
{
class StepTimer
{
    using Clock_t = std::chrono::steady_clock;
    using TimePoint_t = std::chrono::time_point<Clock_t>;

    public:
        static constexpr auto TicksPerSecond = static_cast<double>(Clock_t::period::den) / static_cast<double>(Clock_t::period::num);
        static constexpr auto SecondsPerTick = static_cast<double>(Clock_t::period::num) / static_cast<double>(Clock_t::period::den);

        static constexpr auto TicksToSeconds(uint64_t ticks) noexcept -> double { return static_cast<double>(ticks) / TicksPerSecond; }
        static constexpr auto SecondsToTicks(double seconds) noexcept -> uint64_t { return static_cast<uint64_t>(seconds * TicksPerSecond); }

        // Construct a timer using a variable step
        explicit StepTimer(double maxTimeStep) noexcept
            : m_fixedStepTicks{0ull},
              m_maxDeltaTicks{SecondsToTicks(maxTimeStep)},
              m_useFixedStep{false}
        {
        }

        // Construct a timer using a fixed step
        explicit StepTimer(double timeStep, double maxTimeStep) noexcept
            : m_fixedStepTicks{SecondsToTicks(timeStep)},
              m_maxDeltaTicks{SecondsToTicks(maxTimeStep)},
              m_useFixedStep{true}
        {
        }

        auto GetDeltaTime() const noexcept -> double { return TicksToSeconds(m_deltaTicks); }
        auto GetTotalSeconds() const noexcept -> double { return TicksToSeconds(m_totalTicks); }
        auto GetFramesThisTick() const noexcept -> uint64_t { return m_framesThisTick; }
        auto GetFrameCount() const noexcept -> uint64_t { return m_frameCount; }

        void Reset()
        {
            m_lastTime = std::exchange(m_currentTime, Clock_t::now());
            m_accumulatedTicks = 0ull;
        }

        template<class UpdateFunc>
        auto Tick(const UpdateFunc& update) -> bool
        {
            auto ticks = UpdateTimePoints();
            if (ticks > m_maxDeltaTicks)
                ticks = m_maxDeltaTicks;

            if (m_useFixedStep)
            {
                if (WithinFixedStepEpsilon(ticks))
                {
                    std::cout << "within epsilon: " << ticks << '\n';
                    ticks = m_fixedStepTicks;
                }

                m_accumulatedTicks += ticks;
                m_framesThisTick = 0ull;
                while (m_accumulatedTicks >= m_fixedStepTicks)
                {
                    ++m_framesThisTick;

                    if (m_framesThisTick > 1)
                    {
                        std::cout << "multiple frames: " << m_framesThisTick << '\n';
                    }

                    m_deltaTicks = m_fixedStepTicks;
                    m_totalTicks += m_fixedStepTicks;
                    m_accumulatedTicks -= m_fixedStepTicks;
                    ++m_frameCount;
                    update(static_cast<float>(TicksToSeconds(m_deltaTicks)));
                    return true;
                }
            }
            else
            {
                m_deltaTicks = ticks;
                m_totalTicks += ticks;
                m_accumulatedTicks = 0ull;
                ++m_frameCount;
                m_framesThisTick = 1ull;
                update(static_cast<float>(TicksToSeconds(m_deltaTicks)));
                return true;
            }

            return false;
        }

    private:
        TimePoint_t m_currentTime = Clock_t::now();
        TimePoint_t m_lastTime = Clock_t::now();

        uint64_t m_deltaTicks = 0ull;
        uint64_t m_totalTicks = 0ull;
        uint64_t m_accumulatedTicks = 0ull;

        uint64_t m_frameCount = 0ull;
        uint64_t m_framesThisTick = 0ull;

        uint64_t m_fixedStepTicks;
        uint64_t m_maxDeltaTicks;
        bool m_useFixedStep;

        auto UpdateTimePoints() -> uint64_t
        {
            m_lastTime = std::exchange(m_currentTime, Clock_t::now());
            auto ticks = std::chrono::duration_cast<Clock_t::duration>(m_currentTime - m_lastTime).count();
            return static_cast<uint64_t>(ticks);
        }

        auto WithinFixedStepEpsilon(uint64_t ticks) -> bool
        {
            constexpr auto epsilon = TicksPerSecond / 4000ull;
            const auto delta = std::abs(static_cast<double>(ticks) - static_cast<double>(m_fixedStepTicks));
            return delta < epsilon;
        }
};
} // namespace nc::engine
