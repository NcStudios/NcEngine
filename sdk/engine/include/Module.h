#pragma once

#include <vector>

namespace nc
{
    struct Job;

    /** @brief Base class for modules that attach workloads to the core execution graph. */
    struct Module
    {
        virtual ~Module() = default;

        /**
         * @brief Builds a list of jobs required by the module.
         * @return std::vector<Job>
         */
        virtual auto BuildWorkload() -> std::vector<Job>;

        /** @brief Called on scene changes and NcEngine::Shutdown(). */
        virtual void Clear() noexcept;
    };
}