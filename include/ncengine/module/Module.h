/**
 * @file Module.h
 * @copyright Jaremie and McCallister Romer 2023
 */
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

    /** @brief Called on registered modules prior to loading a new scene. */
    virtual void OnSceneLoad() {}

    /** @brief Called on registered modules prior to clearing the Registry. This
     *         includes Scene::Unload() and NcEngine::Shutdown(). */
    virtual void Clear() noexcept {}
};
} // namespace nc
