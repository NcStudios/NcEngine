#pragma once

#include "ecs/Registry.h"
#include "module/Module.h"
#include "task/Job.h"

namespace nc::task
{
    class Executor
    {
        public:
            template<class T>
            using task_matrix = std::array<std::vector<T>, ExecutionPhaseCount>;

            Executor();

            void BuildTaskGraph(Registry* registry, std::vector<std::unique_ptr<Module>>& modules);
            auto Run() -> tf::Future<void>;

        private:
            tf::Executor m_taskExecutor;
            tf::Taskflow m_executionGraph;
            task_matrix<Job> m_jobRegistry;

            void Add(Job job);
            void Add(std::vector<Job> workload);
            auto ConvertJobsToTasks() -> task_matrix<tf::Task>;
            auto ToTask(Job::single_job_t job) -> tf::Task;
            auto ToTask(Job::multi_job_t job) -> tf::Task;
    };
}