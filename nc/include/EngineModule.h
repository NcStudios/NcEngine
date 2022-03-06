#pragma once

namespace nc
{
    class TaskGraph;

    struct EngineModule
    {
        virtual ~EngineModule() = default;
        // engine_module(const engine_module&) = delete;
        // engine_module(engine_module&&) = default;
        // engine_module& operator=(const engine_module&) = delete;
        // engine_module& operator=(engine_module&&) = default;

        virtual void Clear() noexcept = 0;
        virtual auto GetTasks() -> TaskGraph& = 0;
    };
}