#pragma once

#include "Job.h"
#include "taskflow/taskflow.hpp"

namespace nc
{
    class Executor
    {
        public:
            template<class T>
            using task_matrix = std::array<std::vector<T>, HookPointCount>;

            Executor();
            void Add(Job job);
            void Add(std::vector<Job> workload);
            void Build();
            auto Run() -> tf::Future<void>;

        private:
            tf::Executor m_taskExecutor;
            tf::Taskflow m_executionGraph;
            task_matrix<Job> m_jobRegistry;

            auto ConvertJobsToTasks() -> task_matrix<tf::Task>;
            auto ToTask(Job::single_job_t job) -> tf::Task;
            auto ToTask(Job::multi_job_t job) -> tf::Task;
    };
}