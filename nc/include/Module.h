#pragma once

namespace nc
{
    class TaskGraph;

    /** @brief Base class for modules that attach worloads to the core execution graph. */
    struct Module
    {
        Module() = default;
        virtual ~Module() = default;
        Module(const Module&) = delete;
        Module(Module&&) = default;
        Module& operator=(const Module&) = delete;
        Module& operator=(Module&&) = default;

        /** @brief Called on scene changes and NcEngine::Shutdown(). */
        virtual void Clear() noexcept = 0;

        /** @brief Retrieve the module's workload. */
        virtual auto GetTasks() -> TaskGraph& = 0;
    };
}