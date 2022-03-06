#pragma once

namespace nc
{
    class TaskGraph;

    struct engine_module
    {
        virtual ~engine_module() = default;
        // engine_module(const engine_module&) = delete;
        // engine_module(engine_module&&) = default;
        // engine_module& operator=(const engine_module&) = delete;
        // engine_module& operator=(engine_module&&) = default;

        virtual void clear() noexcept = 0;
        virtual auto get_tasks() -> TaskGraph& = 0;
    };
}