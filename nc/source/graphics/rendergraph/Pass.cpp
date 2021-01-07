#include "Pass.h"
#include "graphics/Graphics.h"

namespace nc::graphics
{
    void Pass::Accept(Job&& job) noexcept
    {
        m_jobs.push_back(std::forward<Job&&>(job));
    }

    void Pass::Execute(Graphics* gfx) const
    {
        for (const auto& job : m_jobs)
        {
            job.Execute(gfx);
        }
    }

    void Pass::Reset() noexcept
    {
        m_jobs.clear();
    }
}