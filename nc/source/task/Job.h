#pragma once

#include "TaskGraph.h"

#include <array>
#include <functional>
#include <variant>
#include <vector>

namespace nc
{
    constexpr size_t HookPointCount = 6ull;

    enum class HookPoint : uint8_t
    {
        Free          = 0,
        Logic         = 1,
        Physics       = 2,
        PreRenderSync = 3,
        Render        = 4,
        PostFrameSync = 5
    };

    struct Job
    {
        using single_job_t = std::function<void()>;
        using multi_job_t = tf::Taskflow*;
        using inner_type = std::variant<single_job_t, multi_job_t>;
        inner_type work;
        const char* name = "unnamed job";
        HookPoint hook;
    };
}