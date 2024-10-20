/**
 * @file NcEngine.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/Events.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/module/ModuleRegistry.h"
#include "ncengine/scene/Scene.h"
#include "ncengine/task/TaskFwd.h"

namespace nc
{
/** @brief Interface for the engine runtime and modules. */
class NcEngine
{
    public:
        virtual ~NcEngine() = default;

        /**
         * @brief Start engine execution.
         * @param initialScene The initial scene to load.
         */
        virtual void Start(std::unique_ptr<Scene> initialScene) = 0;

        /** @brief Signal the engine to stop execution after all tasks have completed. */
        virtual void Stop() noexcept = 0;

        /** @brief Clear all engine state. */
        virtual void Shutdown() noexcept = 0;

        /** @brief Get a reference to the ComponentRegistry. */
        virtual auto GetComponentRegistry() noexcept -> ecs::ComponentRegistry& = 0;

        /** @brief Get a pointer to the module registry. */
        virtual auto GetModuleRegistry() noexcept -> ModuleRegistry* = 0;

        /** @brief Get a reference to the collection of system events. */
        virtual auto GetSystemEvents() noexcept -> SystemEvents& = 0;

        /** @brief Get an interface for running async tasks on the thread pool. */
        virtual auto GetAsyncDispatcher() noexcept -> task::AsyncDispatcher = 0;

        /**
         * @brief Compose a new task graph from all registered modules.
         * 
         * A graph is built on engine initialization. It only needs rebuilding
         * if modules are added afterwards. Only call this from the main thread
         * and when the graph is not currently running (prior to calling Start()
         * or from a Scene).
         */
        virtual void RebuildTaskGraph() = 0;
};

/**
 * @brief Create an NcEngine instance and initialize engine-side modules.
 * 
 * @param config Configuration object specifying engine settings.
 * @return std::unique_ptr<NcEngine>
 */
auto InitializeNcEngine(const config::Config& config) -> std::unique_ptr<NcEngine>;
} // namespace nc
