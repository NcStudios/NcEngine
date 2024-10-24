/**
 * @file Module.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/task/TaskFwd.h"

#include <vector>

namespace nc
{
class ModuleRegistry;
class Registry;

/** @brief Modules are extensions that provide functionality to the engine. */
class Module
{
    public:
        /**
         * @brief Constructor for the Module interface.
         * 
         * All registered modules must have unique ids. Set 'id' to 0 to get assigned an available id
         * upon registration. The range [1, 100] is reserved for engine modules. Automatically assigned
         * ids are assigned in reverse order starting from std::numeric_limits<size_t>::max().
         */
        Module(size_t id = 0) noexcept
            : m_id{id} {}

        virtual ~Module() = default;

        /** @brief Get the module's unique id. */
        auto Id() const noexcept { return m_id; }

        /** @brief Called on registered modules when the task graphs are constructed. */
        virtual void OnBuildTaskGraph(task::UpdateTasks&, task::RenderTasks&) {}

        /** @brief Called on registered modules prior to loading a new scene. */
        virtual void OnBeforeSceneLoad() {}

        /** @brief Called on registered modules immediately before loading a SceneFragment file. */
        virtual void OnBeforeSceneFragmentLoad() {}

        /** @brief Called on registered modules immediately after loading a SceneFragment file. */
        virtual void OnAfterSceneFragmentLoad() {}

        /** @brief Called on registered modules prior to clearing the Registry. This
         *         includes Scene::Unload() and NcEngine::Shutdown(). */
        virtual void Clear() noexcept {}

    private:
        size_t m_id;

        friend class ModuleRegistry;

        void SetId(size_t id) noexcept { m_id = id; }
};
} // namespace nc
