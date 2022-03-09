#include "Executor.h"

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

    void SetDependencies(nc::Executor::task_matrix<tf::Task>& taskList)
    {
        constexpr auto freeIndex = static_cast<size_t>(nc::HookPoint::Free);
        constexpr auto logicIndex = static_cast<size_t>(nc::HookPoint::Logic);
        constexpr auto physicsIndex = static_cast<size_t>(nc::HookPoint::Physics);
        constexpr auto renderIndex = static_cast<size_t>(nc::HookPoint::Render);
        constexpr auto preRenderIndex = static_cast<size_t>(nc::HookPoint::PreRenderSync);
        constexpr auto postFrameIndex = static_cast<size_t>(nc::HookPoint::PostFrameSync);

        auto& freeTasks = taskList[freeIndex];
        auto& logicTasks = taskList[logicIndex];
        auto& physicsTasks = taskList[physicsIndex];
        auto& renderTasks = taskList[renderIndex];
        auto& preRenderTasks = taskList[preRenderIndex];
        auto& postFrameTasks = taskList[postFrameIndex];

        Connect(logicTasks, physicsTasks);
        Connect(physicsTasks, preRenderTasks);
        Connect(freeTasks, preRenderTasks);
        Connect(preRenderTasks, renderTasks);
        Connect(renderTasks, postFrameTasks);
    }
}

namespace nc
{
    Executor::Executor()
        : m_taskExecutor{8u},
          m_executionGraph{},
          m_jobRegistry{}
    {
    }

    void Executor::Add(Job job)
    {
        auto index = static_cast<unsigned>(job.hook);
        m_jobRegistry.at(index).push_back(std::move(job));
    }

    void Executor::Add(std::vector<Job> workload)
    {
        for(auto& job : workload)
        {
            Add(std::move(job));
        }
    }

    void Executor::Build()
    {
        auto tasks = ConvertJobsToTasks();
        SetDependencies(tasks);

        #if NC_OUTPUT_TASKFLOW
        m_executionGraph.name("Main Loop");
        m_executionGraph.dump(std::cout);
        #endif
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