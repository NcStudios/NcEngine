#include "TimeImpl.h"
#include "task/TaskGraph.h"
#include "time/Time.h"
#include "utility/Log.h"

#include <utility>

#include <iostream>

namespace
{
float g_dt = 0.0f;
}

namespace nc::time
{
/** From time/Time.h */
auto DeltaTime() noexcept -> float
{
    return g_dt;
}

void SetDeltaTime(float dt) noexcept
{
    g_dt = dt;
}
} // namespace nc::time
