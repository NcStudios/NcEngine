#include <iostream>
#include "nc/external/include/taskflow/taskflow.hpp"
#include <stdexcept>

#include <exception>
#include <mutex>

void F1()
{
    std::cout << "F1()\n";

    throw std::runtime_error("");
}

void F2()
{
    std::cout << "F2()\n";
}

void F3(int* v)
{
    for(size_t i = 0; i < 10000; ++i)
        *v += i;
    
    std::cout << "F3(): " << *v << "\n";
}

class TaskSharedState
{
    public:
        void StoreException(std::exception_ptr e)
        {
            std::lock_guard lock{m_mutex};
            if(m_exception != nullptr)
                return;

            m_exception = e;
        }

        bool HasException() const
        {
            return m_exception != nullptr;
        }

        void RethrowIfExceptionCaptured()
        {
            if(HasException())
                std::rethrow_exception(m_exception);
        }

    private:
        std::exception_ptr m_exception;
        std::mutex m_mutex;
};


class TaskGraph
{
    public:
        void Run(tf::Executor& executor)
        {
            executor.run(m_tasks).wait();
            m_exceptionState.RethrowIfExceptionCaptured();
        }

        template<class Func>
        auto AddTask(Func&& func) -> tf::Task
        {
            auto task = m_tasks.placeholder();
            task.data(&m_exceptionState);
            task.work
            (
                [&task, func = std::forward<Func>(func)]
                {
                    try
                    {
                        func();
                    }
                    catch(const std::exception& e)
                    {
                        auto* state = static_cast<TaskSharedState*>(task.data());
                        state->StoreException(std::current_exception());
                    }
                }
            );

            return task;
        }

    private:
        tf::Taskflow m_tasks;
        TaskSharedState m_exceptionState;
};

int main()
{
    tf::Executor ex{4u};
    TaskGraph graph;

    int i = 0;

    auto a = graph.AddTask([&i] { F3(&i); });
    auto b = graph.AddTask([] { F2(); });
    auto c = graph.AddTask([] { F1(); });

    graph.Run(ex);


    return 0;
}