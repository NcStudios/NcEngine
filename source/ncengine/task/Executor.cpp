#include "Executor.h"

#include "ncutility/Algorithm.h"
#include "ncutility/NcError.h"
#include "ncutility/ScopeExit.h"

#include <iostream>

namespace
{
// Internal TaskGraph interface
template<class Phase>
class GraphBuilder : public nc::task::TaskGraph<Phase>
{
    public:
        GraphBuilder() = default;

        // Set task dependencies
        void Connect()
        {
            for (auto& [task, id, predecessors, successors] : this->m_tasks)
            {
                for (auto predecessor : predecessors)
                {
                    auto pos = std::ranges::find(this->m_tasks, predecessor, &nc::task::Task::id);
                    if (pos == this->m_tasks.cend())
                    {
                        throw nc::NcError(fmt::format("Did not find predecessor task with id '{}'", predecessor));
                    }

                    pos->task.precede(task);
                }

                for (auto successor : successors)
                {
                    auto pos = std::ranges::find(this->m_tasks, successor, &nc::task::Task::id);
                    if (pos == this->m_tasks.cend())
                    {
                        throw nc::NcError(fmt::format("Did not find successor task with id '{}'", successor));
                    }

                    task.precede(pos->task);
                }
            }
        }

        // Extract TaskGraphContext - leaves this object in an invalid state.
        auto ReleaseContext()
        {
            return std::move(this->m_ctx);
        }
};
} // anonymous namespace

namespace nc::task
{
auto BuildContext(const std::vector<std::unique_ptr<Module>>& modules) -> ExecutorContext
{
    auto updateBuilder = ::GraphBuilder<UpdatePhase>{};
    auto renderBuilder = ::GraphBuilder<RenderPhase>{};
    for (auto& mod : modules)
    {
        mod->OnBuildTaskGraph(updateBuilder, renderBuilder);
    }

    updateBuilder.Connect();
    renderBuilder.Connect();
    return ExecutorContext{updateBuilder.ReleaseContext(), renderBuilder.ReleaseContext()};
}

Executor::Executor(uint32_t threadCount, ExecutorContext ctx)
    : m_executor{threadCount},
      m_ctx{std::move(ctx)}
{
#ifdef NC_OUTPUT_TASKFLOW
    if (m_ctx.update && m_ctx.render)
    {
        WriteGraph(std::cout);
    }
#endif
}

void Executor::SetContext(ExecutorContext ctx)
{
    if (m_runningUpdate || m_runningRender)
    {
        throw NcError{"Cannot set new context while Executor is running."};
    }

    m_ctx = std::move(ctx);

#ifdef NC_OUTPUT_TASKFLOW
    if (m_ctx.update && m_ctx.render)
    {
        WriteGraph(std::cout);
    }
#endif
}

void Executor::RunUpdateTasks()
{
    SCOPE_EXIT(m_runningUpdate = false);
    if (std::exchange(m_runningUpdate, true))
    {
        throw NcError{"Executor is already running update tasks"};
    }

    m_executor.run(m_ctx.update->graph).wait();
    m_ctx.update->exceptionContext.ThrowIfExceptionStored();
}

void Executor::RunRenderTasks()
{
    SCOPE_EXIT(m_runningRender = false);
    if (std::exchange(m_runningRender, true))
    {
        throw NcError{"Executor is already running render tasks"};
    }

    m_executor.run(m_ctx.render->graph).wait();
    m_ctx.render->exceptionContext.ThrowIfExceptionStored();
}

void Executor::WriteGraph(std::ostream& stream) const
{
    stream << "Update Graph:\n";
    m_ctx.update->graph.dump(stream);
    stream << "Render Graph:\n";
    m_ctx.render->graph.dump(stream);
}
} // namespace nc::task
