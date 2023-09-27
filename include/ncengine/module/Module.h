#pragma once

#include <vector>

namespace nc
{
class Registry;

namespace task
{
class TaskGraph;
} // namespace task

/** @brief Modules are extensions that provide functionality to the engine. */
struct Module
{
    virtual ~Module() = default;

    /** @brief Called on registered modules when the task graph is constructed. */
    virtual void OnBuildTaskGraph(task::TaskGraph&) {}

    /** @brief Called prior to clearing the module's associated data registry. This includes
     *         scene changes and NcEngine::Shutdown(). */
    virtual void Clear() noexcept {}
};
} // namespace nc
