/**
 * @file Profile.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#ifdef NC_PROFILING_ENABLED

#include "optick.h"

namespace nc
{
/** @brief Color and filter for a profile event. */
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

/** @brief Profile a function or inner scope. */
#define NC_PROFILE_SCOPE(name, category) OPTICK_CATEGORY(name, category);

/** @brief Profile a task. Use this at the top level of a task instead of NC_PROFILE_SCOPE for proper thread tracking. */
#define NC_PROFILE_TASK(name, category)                                        \
    const auto _ncTaskMeasurement ## __LINE__ = nc::detail::TaskMeasurement{}; \
    NC_PROFILE_SCOPE(name, category);

#else
#define NC_PROFILE_SCOPE(name, category)
#define NC_PROFILE_TASK(name, category)
#endif
