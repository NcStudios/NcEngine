/**
 * @file AsyncDispatcher.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "taskflow/taskflow.hpp"

namespace nc::task
{
/** @brief */
class AsyncDispatcher
{
    public:
        explicit AsyncDispatcher(tf::Executor* executor)
            : m_executor{executor}
        {
        }

        /** @brief */
        template<class F>
        auto Async(F&& f)
        {
            return m_executor->async(std::forward<F>(f));
        }

        /** @brief */
        template<class F>
        void SilentAsync(F&& f)
        {
            m_executor->silent_async(std::forward<F>(f));
        }

        auto MaxConcurrency() const -> size_t
        {
            return m_executor->num_workers();
        }

    private:
        tf::Executor* m_executor;
};
} // namespace nc::task
