#include "JobSystem.h"

namespace nc::job
{
    JobSystem::JobSystem(size_t threadCount)
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

    JobSystem::~JobSystem()
    {
        {
            std::unique_lock<std::mutex> lock{m_mutex};
            m_run = false;
        }

        m_condition.notify_all();

        for(auto& thread : m_threads)
            thread.join();
    }
}