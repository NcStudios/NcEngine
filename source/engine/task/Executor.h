#pragma once

#include "module/Module.h"
#include "task/TaskGraph.h"

#include <iosfwd>

namespace nc::task
{
// Build a graph context from a list of modules.
auto BuildContext(const std::vector<std::unique_ptr<Module>>& modules) -> std::unique_ptr<TaskGraphContext>;

// Manages the engine's primary task graph.
class Executor
{
    public:
        // Initialize with a context object
        explicit Executor(std::unique_ptr<TaskGraphContext> ctx);

        // Assign a new graph context.
        void SetContext(std::unique_ptr<TaskGraphContext> ctx);

        // Blocking call to run the graph. Throws any exceptions caught during execution.
        void Run();

        // Write task graph structure to a stream in Graphviz DOT language.
        void WriteGraph(std::ostream& stream);

    private:
        tf::Executor m_executor;
        std::unique_ptr<TaskGraphContext> m_ctx;
};
} // namespace nc::task
