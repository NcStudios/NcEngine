#pragma once

#include "ncengine/type/StableAddress.h"

#include <concepts>
#include <exception>
#include <mutex>
#include <utility>

namespace nc::task
{
/** @brief State shared between all tasks in a graph for storing an exception. */
class ExceptionContext : StableAddress
{
    public:
        /** @brief Attempt to store the current exception. Only */
        void StoreException(std::exception_ptr e)
        {
            auto lock = std::lock_guard{m_mutex};
            if(m_exception)
            {
                return;
            }

            m_exception = e;
        }

        /**
         * @brief Rethrow an exception caught during task execution, if one was set.
         * @note Only call this from the main thread after the graph has finished.
         */
        void ThrowIfExceptionStored()
        {
            if(m_exception)
            {
                auto e = std::exchange(m_exception, nullptr);
                std::rethrow_exception(e);
            }
        }

    private:
        std::exception_ptr m_exception;
        std::mutex m_mutex;
};

/**
 * @brief Wrap a callable in a try-catch targeting a TaskGraph's ExceptionContext.
 * @tparam F A callable of the form void(*)().
 * @param context The ExceptionContext of the TaskGraph the callable will be scheduled on.
 * @param func The callable to guard.
 * @return A lambda wrapping the callable or the callable itself, depending on the noexcept
 *         specification of func.
 * 
 * Individual tasks added through TaskGraph::Add() are automatically guarded if they do not
 * satisfy std::is_nothrow_invocable. Explicit task guarding is needed only on tasks placed
 * in a tf::Taskflow that will be later composed into the primary TaskGraph.
 */
template<std::invocable<> F>
auto Guard(ExceptionContext& context, F&& func)
{
    if constexpr(std::is_nothrow_invocable_v<F>)
    {
        return std::forward<F>(func);
    }
    else
    {
        return [func = std::forward<F>(func), &context]
        {
            try
            {
                func();
            }
            catch(const std::exception&)
            {
                context.StoreException(std::current_exception());
            }
        };
    }
}
} // namespace nc::task
