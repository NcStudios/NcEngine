#ifndef NCE_TIME
#define NCE_TIME

#include <chrono>

using Clock = std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
//using std::chrono::milliseconds;
using std::chrono::microseconds;

namespace NCE
{
    // class Time
    // {
    //     private:

    //     public:
    //         void UpdateTime();
    //         static int64_t GetDeltaTime();

    //         static time_point<Clock> previous;
    //         static time_point<Clock> current;
    // };

    class Time
    {
        private:
            time_point<Clock> _lastTime;
            time_point<Clock> _currentTime;
            

        public:
            Time();
            void UpdateTime();
            
            static uint64_t lastDelta;
            static uint64_t GetDeltaTime();

    };
    
}



#endif