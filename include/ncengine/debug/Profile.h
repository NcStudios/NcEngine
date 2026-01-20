/**
 * @file Profile.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#ifdef NC_PROFILING_ENABLED

#if defined(NC_USE_TRACY)

#include "tracy/Tracy.hpp"

namespace nc
{
/** @cond internal */
namespace detail
{
struct TaskMeasurement
{
    explicit TaskMeasurement()
    {
        tracy::SetThreadName("NcEngine Worker");
    }

    ~TaskMeasurement() noexcept = default;
};
} // namespace detail
} // namespace nc
/** @endcond */

/** @brief Mark frame boundary. */
#define NC_PROFILE_FRAME(name) FrameMark

/** @brief Profile a function or inner scope. */
#define NC_PROFILE_SCOPE(name, category) ZoneScopedN(name)

/** @brief Profile a task. Use this at the top level of a task instead of NC_PROFILE_SCOPE for proper thread tracking. */
#define NC_PROFILE_TASK(name, category)                                        \
    const auto _ncTaskMeasurement ## __LINE__ = nc::detail::TaskMeasurement{}; \
    NC_PROFILE_SCOPE(name, category)

#elif defined(NC_USE_OPTICK)

#include "optick.h"

namespace nc
{
using ProfileCategory = Optick::Category;

/** @cond internal */
namespace detail
{
struct TaskMeasurement
{
    explicit TaskMeasurement()
    {
        OPTICK_START_THREAD("NcEngine Worker");
    }

    ~TaskMeasurement() noexcept
    {
        OPTICK_STOP_THREAD();
    }
};
} // namespace detail
} // namespace nc
/** @endcond */

/** @brief Mark frame boundary. */
#define NC_PROFILE_FRAME(name) OPTICK_FRAME(name)

/** @brief Profile a function or inner scope. */
#define NC_PROFILE_SCOPE(name, category) OPTICK_CATEGORY(name, category)

/** @brief Profile a task. Use this at the top level of a task instead of NC_PROFILE_SCOPE for proper thread tracking. */
#define NC_PROFILE_TASK(name, category)                                        \
    const auto _ncTaskMeasurement ## __LINE__ = nc::detail::TaskMeasurement{}; \
    NC_PROFILE_SCOPE(name, category)

#endif // NC_USE_TRACY / NC_USE_OPTICK

#else // NC_PROFILING_ENABLED not defined

#define NC_PROFILE_FRAME(name)
#define NC_PROFILE_SCOPE(name, category)
#define NC_PROFILE_TASK(name, category)

#endif // NC_PROFILING_ENABLED
