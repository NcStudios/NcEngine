/**
 * @file TaskGraph.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ExceptionContext.h"

#include "taskflow/taskflow.hpp"
#include "ncutility/NcError.h"

#include <vector>

namespace nc::task
{
/** @brief Context object holding a TaskGraph's state. */
struct TaskGraphContext : StableAddress
{
    tf::Taskflow graph;
    ExceptionContext exceptionContext;
    std::vector<std::unique_ptr<tf::Taskflow>> storage;
};

/** @brief Task state for an item scheduled on a TaskGraph. */
struct Task
{
    tf::Task task;
    size_t id;
    std::vector<size_t> predecessors;
    std::vector<size_t> successors;
};

/** @brief Task graph interface for building a TaskGraphContext with Module tasks. */
template<class Phase>
class TaskGraph
{
    public:
        /**
         * @brief Schedule a single task to run during a phase.
         * @tparam F A callable of the form void(*)().
         * @param id A unique id for the task.
         * @param name A user-friendly name for the task.
         * @param func The callable to schedule.
         * @param predecessors An optional list of task ids to be scheduled before the task.
         * @param successors An optional list of task ids to be scheduled after the task.
         * @return A handle to a scheduled task.
         * @note If func doesn't satisfy std::is_nothrow_invocable, it will be
         *       wrapped with a call to task::Guard().
         */
        template<std::invocable<> F>
        auto Add(size_t id,
                 std::string_view name,
                 F&& func,
                 std::vector<size_t> predecessors = {},
                 std::vector<size_t> successors = {}) -> tf::Task
        {
            return Schedule(id, Emplace(name, std::forward<F>(func)), std::move(predecessors), std::move(successors));
        }

        /**
         * @brief Schedule a tf::Taskflow to run during a phase.
         * @param id A unique id for the task.
         * @param name A user-friendly name for the task graph.
         * @param graph The graph to be composed.
         * @param predecessors An optional list of task ids to be scheduled before the task.
         * @param successors An optional list of task ids to be scheduled after the task.
         * @return A handle to a scheduled task composed from the Taskflow.
         * @note Ensure exceptions cannot leak from the graph. Tasks may be
         *       wrapped with task::Guard() to delay throwing until execution
         *       has finished.
         */
        auto Add(size_t id,
                 std::string_view name,
                 std::unique_ptr<tf::Taskflow> graph,
                 std::vector<size_t> predecessors = {},
                 std::vector<size_t> successors = {}) -> tf::Task
        {
            NC_ASSERT(graph != nullptr, "Task graph should not be null.");
            return Schedule(id, Emplace(name, std::move(graph)), std::move(predecessors), std::move(successors));
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
        std::vector<Task> m_tasks;

        TaskGraph()
            : m_ctx{std::make_unique<TaskGraphContext>()}
        {
        }

        ~TaskGraph() noexcept = default;

        auto Schedule(size_t id, tf::Task handle, std::vector<size_t> predecessors, std::vector<size_t> successors) -> tf::Task
        {
            return m_tasks.emplace_back(handle, id, std::move(predecessors), std::move(successors)).task;
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

/** @brief Tag type to identify the update graph. */
struct UpdatePhase;

/** @brief Tag type to identify the render graph. */
struct RenderPhase;

/** @brief Alias for the TaskGraph handling update tasks. */
using UpdateTasks = TaskGraph<UpdatePhase>;

/** @brief Alias for the TaskGraph handling render tasks. */
using RenderTasks = TaskGraph<RenderPhase>;
} // namespace nc::task
