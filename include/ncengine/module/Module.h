#pragma once

#include <vector>

namespace nc
{
    namespace task { struct Job; }

    /** @brief Modules are extensions that provide functionality to the engine. */
    struct Module
    {
        virtual ~Module() = default;

        /**
         * @brief Builds a list of jobs required by the module. Returns an empty vector by default.
         * @return std::vector<task::Job>
         */
        virtual auto BuildWorkload() -> std::vector<task::Job>;

        /** @brief Called prior to clearing the module's associated data registry. This includes
         *  scene changes and NcEngine::Shutdown(). */
        virtual void Clear() noexcept;
    };
}