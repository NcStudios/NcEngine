/**
 * @file ScopeExit.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

/** @cond internal */
namespace nc::detail
{
template <class F>
class ScopeExit
{
public:
    explicit ScopeExit(const F& onExit) noexcept
        : m_func{onExit}
    {
    }

    explicit ScopeExit(F&& onExit) noexcept
        : m_func{std::move(onExit)}
    {
    }

    ~ScopeExit() noexcept
    {
        m_func();
    }

    ScopeExit(ScopeExit&&) = delete;
    ScopeExit(const ScopeExit&) = delete;
    void operator=(const ScopeExit&) = delete;
    void operator=(ScopeExit&&) = delete;

private:
    F m_func;
};
} // namespace nc::detail

#define UNIQUE_NAME_HELPER(name, lineNumber) name ## lineNumber
#define UNIQUE_NAME(name, lineNumber) UNIQUE_NAME_HELPER(name, lineNumber)
/** @endcond internal */

/** @brief Create an RAII object in the current stack scope that executes 'body' on destruction. */
#define SCOPE_EXIT(body) auto UNIQUE_NAME(localScopeExitFunc, __LINE__) = nc::detail::ScopeExit([&](){body;});
