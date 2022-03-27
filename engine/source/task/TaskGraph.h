#pragma once

#include "taskflow/taskflow/taskflow.hpp"

#include <exception>
#include <mutex>

#define NC_OUTPUT_TASKFLOW 0

namespace nc
{
    /** State shared between all tasks in a graph for storing
     *  an exception. */
    class TaskExceptionState
    {
        public:
            void StoreException(std::exception_ptr e);
            void ThrowIfExceptionStored();
            bool HasException() const;

        private:
            std::exception_ptr m_exception;
            std::mutex m_mutex;
    };

    /** Wrapper around tf::Taskflow that allow try/catch guarding of tasks.
     *  Only the first exception thrown may be stored. */
    class TaskGraph
    {
        public:
            /** Blocking task graph execution. A stored exception will be
             *  thrown after all tasks have finished. */
            void Run(tf::Executor& executor);

            /** Non-blocking task graph execution. For a graph containing
             *  guarded tasks, ThrowIfExceptionStored() should be called
             *  manually after execution has finished. */
            auto RunAsync(tf::Executor& executor) -> tf::Future<void>;

            /** Rethrows the first captured exception from a task, if
             *  one exists. */
            void ThrowIfExceptionStored();

            /** Add a task enclosed in a try/catch guard to the graph. */
            template<class Func>
            auto AddGuardedTask(Func&& func) -> tf::Task;

            /** Get underlying Taskflow object for adding unguarded tasks, 
             *  composing, etc. */
            auto GetTaskFlow() -> tf::Taskflow* { return &m_tasks; }

        private:
            tf::Taskflow m_tasks;
            TaskExceptionState m_exceptionState;
    };

    inline void TaskExceptionState::StoreException(std::exception_ptr e)
    {
        std::lock_guard lock{m_mutex};
        if(HasException())
            return;

        m_exception = e;
    }

    inline bool TaskExceptionState::HasException() const
    {
        return m_exception != nullptr;
    }

    inline void TaskExceptionState::ThrowIfExceptionStored()
    {
        if(HasException())
            std::rethrow_exception(m_exception);
    }

    inline void TaskGraph::Run(tf::Executor& executor)
    {
        executor.run(m_tasks).wait();
        m_exceptionState.ThrowIfExceptionStored();
    }

    inline auto TaskGraph::RunAsync(tf::Executor& executor) -> tf::Future<void>
    {
        return executor.run(m_tasks);
    }

    inline void TaskGraph::ThrowIfExceptionStored()
    {
        m_exceptionState.ThrowIfExceptionStored();
    }

    template<class Func>
    auto TaskGraph::AddGuardedTask(Func&& func) -> tf::Task
    {
        auto task = m_tasks.placeholder();
        task.data(&m_exceptionState);
        task.work
        (
            [task, func = std::forward<Func>(func)]
            {
                try
                {
                    func();
                }
                catch(const std::exception&)
                {
                    auto* state = static_cast<TaskExceptionState*>(task.data());
                    state->StoreException(std::current_exception());
                }
            }
        );

        return task;
    }
} // namespace nc