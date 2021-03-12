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

    inline JobSystem::JobSystem(size_t threadCount)
        : m_threads{},
          m_jobs{},
          m_mutex{},
          m_condition{},
          m_run{true}
    {
        m_threads.reserve(threadCount);
        std::generate_n(std::back_inserter(m_threads), threadCount, [this]()
        {
            return std::thread{ [this]()
            {
                for(;;)
                {
                    std::packaged_task<void()> job;

                    {
                        std::unique_lock<std::mutex> lock(m_mutex);
                        m_condition.wait(lock, [this](){ return !m_run || !m_jobs.empty(); });
                        if(!m_run && m_jobs.empty())
                            return;

                        job = std::move(m_jobs.front());
                        m_jobs.pop();
                    }

                    job();
                }
            } };
        });
    }

    inline JobSystem::~JobSystem()
    {
        {
            std::unique_lock<std::mutex> lock{m_mutex};
            m_run = false;
        }

        m_condition.notify_all();

        for(auto& thread : m_threads)
            thread.join();
    }

    template<class Func, class... Args>
    auto JobSystem::Schedule(Func&& func, Args&&... args) -> std::future<result_t<Func(Args...)>>
    {
        auto task = Package(std::forward<Func>(func), std::forward<Args>(args)...);
        auto res = task.get_future();

        {
            std::unique_lock<std::mutex> lock{m_mutex};

            if(!m_run)
                throw std::runtime_error("JobSystem::Schedule - Cannot Schedule if not running");

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