#include "Time.hpp"

namespace NCE
{

    // time_point<Clock> Time::previous = Clock::now();
    // time_point<Clock> Time::current = Clock::now();

    // void Time::UpdateTime()
    // {
    //     previous = current;
    //     current  = Clock::now();
    // }

    // int64_t Time::GetDeltaTime()
    // {
    //     microseconds delta = duration_cast<microseconds>(Time::current - Time::previous);
    //     return delta.count();
    // }

    uint64_t Time::lastDelta = 0;

    Time::Time()
    {
        _lastTime = Clock::now();
        _currentTime = Clock::now();
    }

    void Time::UpdateTime()
    {
        Time::lastDelta = duration_cast<microseconds>(_currentTime - _lastTime).count();
        _lastTime = _currentTime;
        _currentTime  = Clock::now();
    }

    uint64_t Time::GetDeltaTime()
    {
        return Time::lastDelta;
    }


}