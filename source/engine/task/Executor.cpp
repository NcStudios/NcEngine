#include "Executor.h"

#include "ncutility/Algorithm.h"
#include "ncutility/NcError.h"
#include "ncutility/ScopeExit.h"

#include <iostream>

namespace
{
constexpr auto g_threadCount = size_t{8};

void SetDependencies(std::vector<tf::Task>& predecessors, const std::vector<tf::Task>& successors)
{
    for(auto& before : predecessors)
    {
        for(const auto& after : successors)
        {
            before.precede(after);
        }
    }
}

// Internal TaskGraph interface
template<class Phase>
class GraphBuilder : public nc::task::TaskGraph<Phase>
{
    public:
        GraphBuilder() = default;

        // Add necessary placeholder tasks. Call after adding tasks but before
        // calling Connect().
        void EnsureNoEmptyBuckets()
        {
            for (auto [i, bucket] : nc::algo::Enumerate(this->m_taskBuckets))
            {
                if (bucket.empty())
                {
                    const auto phase = static_cast<Phase>(i);
                    this->Add(phase, "Placeholder Task", []() noexcept {});
                }
            }
        }

        // Set task dependencies between phases
        void Connect(Phase before, Phase after)
        {
            SetDependencies(Bucket(before), Bucket(after));
        }

        // Extract TaskGraphContext - leaves this object in an invalid state.
        auto ReleaseContext()
        {
            return std::move(this->m_ctx);
        }

    private:
        auto Bucket(Phase phase) -> std::vector<tf::Task>&
        {
            return this->m_taskBuckets.at(static_cast<size_t>(phase));
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

    updateBuilder.EnsureNoEmptyBuckets();
    updateBuilder.Connect(UpdatePhase::Begin, UpdatePhase::Free);
    updateBuilder.Connect(UpdatePhase::Begin, UpdatePhase::Logic);
    updateBuilder.Connect(UpdatePhase::Logic, UpdatePhase::Physics);
    updateBuilder.Connect(UpdatePhase::Physics, UpdatePhase::Sync);
    updateBuilder.Connect(UpdatePhase::Free, UpdatePhase::Sync);

    renderBuilder.EnsureNoEmptyBuckets();
    renderBuilder.Connect(RenderPhase::Render, RenderPhase::PostRender);

    return ExecutorContext{updateBuilder.ReleaseContext(), renderBuilder.ReleaseContext()};
}

Executor::Executor(ExecutorContext ctx)
    : m_executor{g_threadCount},
      m_ctx{std::move(ctx)}
{
#ifdef NC_OUTPUT_TASKFLOW
    WriteGraph(std::cout);
#endif
}

void Executor::SetContext(ExecutorContext ctx)
{
    if (m_runningUpdate || m_runningRender)
    {
        throw NcError{"Cannot set new context while Executor is running."};
    }

    m_ctx = std::move(ctx);
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
