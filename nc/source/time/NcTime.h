#pragma once
#include <chrono>

namespace nc::time
{
    class Time
    {
        public:
            Time() noexcept;

            double GetFixedDeltaTime() const noexcept;
            double GetFrameDeltaTime() const noexcept;
            void ResetFixedDeltaTime() noexcept;
            void ResetFrameDeltaTime() noexcept;
            void UpdateTime() noexcept;

        private:
            using Clock = std::chrono::high_resolution_clock;
            std::chrono::time_point<Clock> m_lastTime;
            std::chrono::time_point<Clock> m_currentTime;
            double m_fixedDeltaTime;
            double m_frameDeltaTime;
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
