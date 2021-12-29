#pragma once

#include <chrono>

namespace nc::time
{
    class Time
    {
        public:
            Time() noexcept;

            auto GetFrameDeltaTime() const noexcept -> float { return static_cast<float>(m_frameDeltaTime); }
            auto GetAccumulatedTime() const noexcept -> float { return static_cast<float>(m_accumulatedTime); }
            void DecrementAccumulatedTime(float time) noexcept { m_accumulatedTime -= time; }
            void ResetFrameDeltaTime() noexcept { m_frameDeltaTime = 0.0; }
            void ResetAccumulatedTime() noexcept { m_accumulatedTime = 0.0; }
            auto UpdateTime() noexcept -> float;

        private:
            using Clock = std::chrono::high_resolution_clock;
            std::chrono::time_point<Clock> m_lastTime;
            std::chrono::time_point<Clock> m_currentTime;
            double m_frameDeltaTime;
            double m_accumulatedTime;
    };

    class Timer
    {
        public:
            void  Start() noexcept;
            void  Stop() noexcept;
            float Value() const noexcept;

        private:
            using Clock = std::chrono::high_resolution_clock;
            float m_time = 0.0f;
            std::chrono::time_point<Clock> m_start;
            std::chrono::time_point<Clock> m_end;
    };
} //end namespace nc::time