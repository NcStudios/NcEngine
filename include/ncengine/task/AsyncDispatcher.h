/**
 * @file AsyncDispatcher.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "taskflow/taskflow.hpp"

namespace nc::task
{
/** @brief Dispatcher for running tasks on the thread pool outside of a TaskGraph. */
class AsyncDispatcher
{
    public:
        explicit AsyncDispatcher(tf::Executor* executor)
            : m_executor{executor}
        {
        }

        /** @brief Run a function asynchronously, returning the eventual result in a std::future. */
        template<class F>
        auto Async(F&& f) -> std::future<std::invoke_result_t<std::decay_t<F>>>
        {
            return m_executor->async(std::forward<F>(f));
        }

        /** @brief Run a function asynchronously without returning the result. */
        template<class F>
        void SilentAsync(F&& f)
        {
            m_executor->silent_async(std::forward<F>(f));
        }

        /** @brief Get the number of workers in the thread pool. */
        auto MaxConcurrency() const -> size_t
        {
            return m_executor->num_workers();
        }

    private:
        tf::Executor* m_executor;
};
} // namespace nc::task
