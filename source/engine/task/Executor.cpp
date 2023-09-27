#include "Executor.h"

#include "optick/optick.h"
#include "ncutility/Algorithm.h"

#include <ostream>

namespace
{
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
class GraphBuilder : public nc::task::TaskGraph
{
    public:
        GraphBuilder() = default;

        // Add necessary placeholder tasks. Call after adding tasks but before
        // calling Connect().
        void EnsureNoEmptyBuckets()
        {
            for (auto [i, bucket] : nc::algo::Enumerate(m_taskBuckets))
            {
                if (bucket.empty())
                {
                    const auto phase = static_cast<nc::task::ExecutionPhase>(i);
                    Add(phase, "Placeholder Task", []() noexcept {});
                }
            }
        }

        // Set task dependencies between phases
        void Connect(nc::task::ExecutionPhase before, nc::task::ExecutionPhase after)
        {
            SetDependencies(Bucket(before), Bucket(after));
        }

        // Extract TaskGraphContext - leaves this object in an invalid state.
        auto ReleaseContext()
        {
            return std::move(m_ctx);
        }

    private:
        auto Bucket(nc::task::ExecutionPhase phase) -> std::vector<tf::Task>&
        {
            return m_taskBuckets.at(static_cast<size_t>(phase));
        }
};
} // anonymous namespace

namespace nc::task
{
auto BuildContext(const std::vector<std::unique_ptr<Module>>& modules) -> std::unique_ptr<TaskGraphContext>
{
    auto builder = ::GraphBuilder{};
    for (auto& mod : modules)
    {
        mod->OnBuildTaskGraph(builder);
    }

    builder.EnsureNoEmptyBuckets();
    builder.Connect(ExecutionPhase::Begin, ExecutionPhase::Free);
    builder.Connect(ExecutionPhase::Begin, ExecutionPhase::Logic);
    builder.Connect(ExecutionPhase::Logic, ExecutionPhase::Physics);
    builder.Connect(ExecutionPhase::Physics, ExecutionPhase::PreRenderSync);
    builder.Connect(ExecutionPhase::Free, ExecutionPhase::PreRenderSync);
    builder.Connect(ExecutionPhase::PreRenderSync, ExecutionPhase::Render);
    builder.Connect(ExecutionPhase::Render, ExecutionPhase::PostFrameSync);
    return builder.ReleaseContext();
}

Executor::Executor(std::unique_ptr<TaskGraphContext> ctx)
    : m_executor{8},
      m_ctx{std::move(ctx)}
{
}

void Executor::SetContext(std::unique_ptr<TaskGraphContext> ctx)
{
    m_ctx = std::move(ctx);
}

void Executor::Run()
{
    m_executor.run(m_ctx->graph).wait();
    m_ctx->exceptionContext.ThrowIfExceptionStored();
}

void Executor::WriteGraph(std::ostream& stream)
{
    m_ctx->graph.dump(stream);
}
} // namespace nc::task
