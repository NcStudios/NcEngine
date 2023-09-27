#pragma once

#include "ExceptionContext.h"

#include "taskflow/taskflow.hpp"
#include "ncutility/NcError.h"

#include <array>
#include <vector>

namespace nc::task
{
/** @brief The number of execution phases in the primary task graph. */
constexpr size_t ExecutionPhaseCount = 7ull;

/** @brief Identifies an execution phase in the engine's primary task graph. */
enum class ExecutionPhase : uint8_t
{
    Begin,         // First phase to run
    Free,          // Runs parallel to Logic and Physics
    Logic,         // First update phase
    Physics,       // Second update phase - depends on Logic
    PreRenderSync, // Point at which Registry changes are committed - depends on Free and Physics
    Render,        // Rendering phase - depends on PreRenderSync
    PostFrameSync  // Last phase to run
};

/** @brief Context object holding a TaskGraph's state. */
struct TaskGraphContext : StableAddress
{
    tf::Taskflow graph;
    ExceptionContext exceptionContext;
    std::vector<std::unique_ptr<tf::Taskflow>> storage;
};

/** @brief Task graph interface for building a TaskGraphContext with Module tasks. */
class TaskGraph
{
    public:
        /**
         * @brief Schedule a single task to run during a phase.
         * @tparam F A callable of the form `void()`
         * @param phase The target phase.
         * @param name A user-friendly name for the task.
         * @param func A callable of the form void(*)().
         * @return A handle to a scheduled task.
         * @note If func doesn't satisfy std::is_nothrow_invocable, it will be
         *       wrapped with a call to task::Guard().
         */
        template<std::invocable<> F>
        auto Add(ExecutionPhase phase, std::string_view name, F&& func) -> tf::Task
        {
            return Schedule(phase, Emplace(name, std::forward<F>(func)));
        }

        /**
         * @brief Schedule a tf::Taskflow to run during a phase.
         * @param phase The target phase.
         * @param name A user-friendly name for the task.
         * @param graph The graph to be composed.
         * @return A handle to a scheduled task composed of the graph.
         * @note Ensure exceptions cannot leak from the graph. Tasks may be
         *       wrapped with task::Guard() to delay throwing until execution
         *       has finished.
         */
        auto Add(ExecutionPhase phase, std::string_view name, std::unique_ptr<tf::Taskflow> graph) -> tf::Task
        {
            NC_ASSERT(graph != nullptr, "Task graph should not be null.");
            return Schedule(phase, Emplace(name, std::move(graph)));
        }

        /** @brief Take ownership of a tf::Tasflow without scheduling anything. Useful for
         *         keeping alive Taskflows that are indirectly refenced in the TaskGraph. */
        void StoreGraph(std::unique_ptr<tf::Taskflow> graph)
        {
            m_ctx->storage.push_back(std::move(graph));
        }

        /** @brief Get the TaskGraph's ExceptionContext object. */
        auto GetExceptionContext() noexcept -> ExceptionContext&
        {
            return m_ctx->exceptionContext;
        }

    protected:
        std::unique_ptr<TaskGraphContext> m_ctx;
        std::array<std::vector<tf::Task>, ExecutionPhaseCount> m_taskBuckets;

        TaskGraph()
            : m_ctx{std::make_unique<TaskGraphContext>()}
        {
        }

        ~TaskGraph() noexcept = default;

        auto Schedule(ExecutionPhase phase, tf::Task handle) -> tf::Task
        {
            return m_taskBuckets.at(static_cast<size_t>(phase)).emplace_back(handle);
        }

        template<std::invocable<> F>
        auto Emplace(std::string_view name, F&& func) -> tf::Task
        {
            return m_ctx->graph.emplace(Guard(m_ctx->exceptionContext, std::forward<F>(func))).name(name.data());
        }

        auto Emplace(std::string_view name, std::unique_ptr<tf::Taskflow> graph) -> tf::Task
        {
            auto handle = m_ctx->graph.composed_of(*graph).name(name.data());
            m_ctx->storage.emplace_back(std::move(graph));
            return handle;
        }
};
} // namespace nc::task
