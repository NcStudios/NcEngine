#pragma once
#include <chrono>
#include <stdint.h>

using Clock = std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::microseconds;

namespace nc::time
{
    class Time
    {
        public:
            Time();
            void UpdateTime();
            void ResetFixedDeltaTime();
            void ResetFrameDeltaTime();

            static uint64_t CycleDeltaTime; //microseconds since last time through main loop 
            static double FixedDeltaTime;   //seconds since last fixed update
            static double FrameDeltaTime;   //seconds since last frame update
            static uint64_t FrameRenderTime;//microseconds to render last frame

        private:
            time_point<Clock> m_lastTime;
            time_point<Clock> m_currentTime;
    };

    class Timer
    {
        public:
            void  Start();
            void  Stop();
            float Value() const;

        private:
            float m_time = 0.0f;
            time_point<Clock> m_start;
            time_point<Clock> m_end;
    };
} //end namespace nc::time
