#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace nc::job
{
    /** @todo JobSystem is created with 2 threads (Engine.cpp) because that's all we're using.
     *  As things get more complicated, we'll have to change this. Ideally hardware_concurrency
     *  minus other threads should do the trick, but what do we do about the potential for 
     *  unexpected return values, and in what circumstances can they happen? */

    class JobSystem
    {
        template<class T>
        using result_t = std::result_of_t<T>;
        
        public:
            JobSystem(size_t threadCount);
            ~JobSystem();

            template<class Func, class... Args>
            auto Schedule(Func&& func, Args&&... args) -> std::future<result_t<Func(Args...)>>;

        private:
            std::vector<std::thread> m_threads;
            std::queue<std::packaged_task<void()>> m_jobs;
            std::mutex m_mutex;
            std::condition_variable m_condition;
            bool m_run;

            template<class Func>
            auto Package(Func&& func) -> std::packaged_task<result_t<Func()>()>;

            template<class Func, class... Args>
            auto Package(Func&& func, Args&&... args) -> std::packaged_task<result_t<Func(Args...)>()>;
    };

    template<class Func, class... Args>
    auto JobSystem::Schedule(Func&& func, Args&&... args) -> std::future<result_t<Func(Args...)>>
    {
        auto task = Package(std::forward<Func>(func), std::forward<Args>(args)...);
        auto res = task.get_future();

        {
            std::unique_lock<std::mutex> lock{m_mutex};
            m_jobs.emplace(std::move(task));
        }

        m_condition.notify_one();
        return res;
    }

    template<class Func>
    auto JobSystem::Package(Func&& func) -> std::packaged_task<result_t<Func()>()>
    {
        return std::packaged_task<result_t<Func()>()>{std::forward<Func>(func)};
    }

    template<class Func, class... Args>
    auto JobSystem::Package(Func&& func, Args&&... args) -> std::packaged_task<result_t<Func(Args...)>()>
    {
        return std::packaged_task<result_t<Func(Args...)>()>
        {
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
        };
    }
} // namespace nc::job