/**
 * @file TaskGraph.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ExceptionContext.h"

#include "taskflow/taskflow.hpp"
#include "ncutility/NcError.h"

#include <array>
#include <vector>

namespace nc::task
{
/**
 * @brief Identifies a phase in the engine's update task graph.
 *
 * Update loop execution is broken up into phases. Tasks and task graphs are
 * scheduled to run during particular phases. Generally, phases execute
 * sequentially, with the exception of Free, which runs parallel to other
 * update phases.
 */
enum class UpdatePhase : uint8_t
{
    Begin,         // First phase to run
    Free,          // Runs parallel to Logic and Physics
    Logic,         // Update FrameLogic components
    Physics,       // Run NcPhysics task graph and update FixedLogic components - depends on Logic
    Sync,          // Commit Registry changes - depends on Free and Physics
    Count          // Enum size helper (not a phase)
};

/**
 * @brief Identifies a phase in the engine's render task graph.
 * 
 * Render phases run sequentially after all update phases have finished.
 */
enum class RenderPhase : uint8_t
{
    Render,        // Host-side rendering logic
    PostRender,    // Runs after Render
    Count          // Enum size helper (not a phase)
};

/** @brief Context object holding a TaskGraph's state. */
struct TaskGraphContext : StableAddress
{
    tf::Taskflow graph;
    ExceptionContext exceptionContext;
    std::vector<std::unique_ptr<tf::Taskflow>> storage;
};

/** @brief Task graph interface for building a TaskGraphContext with Module tasks. */
template<class Phase>
class TaskGraph
{
    public:
        /**
         * @brief Schedule a single task to run during a phase.
         * @tparam F A callable of the form void(*)().
         * @param phase The target phase.
         * @param name A user-friendly name for the task.
         * @param func The callable to schedule.
         * @return A handle to a scheduled task.
         * @note If func doesn't satisfy std::is_nothrow_invocable, it will be
         *       wrapped with a call to task::Guard().
         */
        template<std::invocable<> F>
        auto Add(Phase phase, std::string_view name, F&& func) -> tf::Task
        {
            return Schedule(phase, Emplace(name, std::forward<F>(func)));
        }

        /**
         * @brief Schedule a tf::Taskflow to run during a phase.
         * @param phase The target phase.
         * @param name A user-friendly name for the task graph.
         * @param graph The graph to be composed.
         * @return A handle to a scheduled task composed from the Taskflow.
         * @note Ensure exceptions cannot leak from the graph. Tasks may be
         *       wrapped with task::Guard() to delay throwing until execution
         *       has finished.
         */
        auto Add(Phase phase, std::string_view name, std::unique_ptr<tf::Taskflow> graph) -> tf::Task
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
        std::array<std::vector<tf::Task>, static_cast<size_t>(Phase::Count)> m_taskBuckets;

        TaskGraph()
            : m_ctx{std::make_unique<TaskGraphContext>()}
        {
        }

        ~TaskGraph() noexcept = default;

        auto Schedule(Phase phase, tf::Task handle) -> tf::Task
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

/** @brief Alias for the TaskGraph handling update tasks. */
using UpdateTasks = TaskGraph<UpdatePhase>;

/** @brief Alias for the TaskGraph handling render tasks. */
using RenderTasks = TaskGraph<RenderPhase>;
} // namespace nc::task
