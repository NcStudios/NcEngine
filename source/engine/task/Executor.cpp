#include "Executor.h"

#include "optick/optick.h"

namespace
{
    void Connect(std::vector<tf::Task>& predecessors, const std::vector<tf::Task>& successors)
    {
        for(auto& before : predecessors)
        {
            for(const auto& after : successors)
            {
                before.precede(after);
            }
        }
    }

    void SetDependencies(nc::task::Executor::task_matrix<tf::Task>& taskList)
    {
        constexpr auto beginIndex = static_cast<size_t>(nc::task::ExecutionPhase::Begin);
        constexpr auto freeIndex = static_cast<size_t>(nc::task::ExecutionPhase::Free);
        constexpr auto logicIndex = static_cast<size_t>(nc::task::ExecutionPhase::Logic);
        constexpr auto physicsIndex = static_cast<size_t>(nc::task::ExecutionPhase::Physics);
        constexpr auto renderIndex = static_cast<size_t>(nc::task::ExecutionPhase::Render);
        constexpr auto preRenderIndex = static_cast<size_t>(nc::task::ExecutionPhase::PreRenderSync);
        constexpr auto postFrameIndex = static_cast<size_t>(nc::task::ExecutionPhase::PostFrameSync);

        auto& beginTasks = taskList[beginIndex];
        auto& freeTasks = taskList[freeIndex];
        auto& logicTasks = taskList[logicIndex];
        auto& physicsTasks = taskList[physicsIndex];
        auto& renderTasks = taskList[renderIndex];
        auto& preRenderTasks = taskList[preRenderIndex];
        auto& postFrameTasks = taskList[postFrameIndex];

        Connect(beginTasks, freeTasks);
        Connect(beginTasks, logicTasks);
        Connect(logicTasks, physicsTasks);
        Connect(physicsTasks, preRenderTasks);
        Connect(freeTasks, preRenderTasks);
        Connect(preRenderTasks, renderTasks);
        Connect(renderTasks, postFrameTasks);
    }
}

namespace nc::task
{
    Executor::Executor()
        : m_taskExecutor{8u},
          m_executionGraph{},
          m_jobRegistry{}
    {
    }

    void Executor::BuildTaskGraph(Registry* registry, std::vector<std::unique_ptr<Module>>& modules)
    {
        for(auto& module : modules)
        {
            Add(module->BuildWorkload());
        }

        auto syncJob = [registry]
        {
            OPTICK_CATEGORY("Sync State", Optick::Category::None);
            registry->CommitStagedChanges();
        };

        Add(Job{ syncJob, "Sync", ExecutionPhase::PreRenderSync });
        auto tasks = ConvertJobsToTasks();
        SetDependencies(tasks);

        #if NC_OUTPUT_TASKFLOW
        m_executionGraph.name("Main Loop");
        m_executionGraph.dump(std::cout);
        #endif
    }

    void Executor::Add(Job job)
    {
        auto index = static_cast<unsigned>(job.phase);
        m_jobRegistry.at(index).push_back(std::move(job));
    }

    void Executor::Add(std::vector<Job> workload)
    {
        for(auto& job : workload)
        {
            Add(std::move(job));
        }
    }

    auto Executor::Run() -> tf::Future<void>
    {
        return m_taskExecutor.run(m_executionGraph);
    }

    auto Executor::ConvertJobsToTasks() -> task_matrix<tf::Task>
    {
        task_matrix<tf::Task> out;
        size_t index = 0ull;

        for(auto& jobList : m_jobRegistry)
        {
            for(auto& [job, name, unused] : jobList)
            {
                auto task = std::visit([this](auto&& work) { return ToTask(work); }, job);
                task.name(name);
                out.at(index).push_back(task);
            }

            ++index;
        }

        return out;
    }

    auto Executor::ToTask(Job::single_job_t job) -> tf::Task
    {
        return m_executionGraph.emplace(std::move(job));
    }

    auto Executor::ToTask(Job::multi_job_t job) -> tf::Task
    {
        return m_executionGraph.composed_of(*job);
    }
}