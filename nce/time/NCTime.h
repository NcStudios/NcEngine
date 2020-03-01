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
        private:
            time_point<Clock> m_lastTime;
            time_point<Clock> m_currentTime;
            
        public:
            Time();
            void UpdateTime();
            void ResetFixedDeltaTime();
            void ResetFrameDeltaTime();
            static uint64_t CycleDeltaTime; //microseconds since last time through main loop 
            static double FixedDeltaTime;   //seconds since last fixed update
            static double FrameDeltaTime;   //seconds since last frame update
    };
} //end namespace nc::time
