#pragma once

#include "module/Module.h"
#include "task/TaskGraph.h"

#include <iosfwd>

namespace nc::task
{
// Executor state consisting of both update and render contexts
struct ExecutorContext
{
    std::unique_ptr<TaskGraphContext> update;
    std::unique_ptr<TaskGraphContext> render;
};

// Build a graph context from a list of modules.
auto BuildContext(const std::vector<std::unique_ptr<Module>>& modules) -> ExecutorContext;

// Manages the engine's primary task graph.
class Executor
{
    public:
        // Initialize with a context object
        explicit Executor(uint32_t threadCount, ExecutorContext ctx);

        // Assign a new graph context.
        void SetContext(ExecutorContext ctx);

        // Check if graph contexts are valid.
        auto IsContextInitialized() const noexcept { return m_ctx.update && m_ctx.render; }

        // Blocking call to run the update graph. Throws any exceptions caught during execution.
        void RunUpdateTasks();

        // Blocking call to run the render graph. Throws any exceptions caught during execution.
        void RunRenderTasks();

        // Write task graph structure to a stream in Graphviz DOT language.
        void WriteGraph(std::ostream& stream) const;

    private:
        tf::Executor m_executor;
        ExecutorContext m_ctx;
        bool m_runningUpdate = false;
        bool m_runningRender = false;
};
} // namespace nc::task
