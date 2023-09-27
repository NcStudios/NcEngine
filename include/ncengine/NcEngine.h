#pragma once

#include "config/Config.h"
#include "ecs/Registry.h"
#include "module/ModuleRegistry.h"
#include "scene/Scene.h"

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

        /**
         * @brief Signal the engine to stop execution after all tasks have completed.
         */
        virtual void Stop() noexcept = 0;

        /**
         * @brief Clear all engine state.
         */
        virtual void Shutdown() noexcept = 0;

        /**
         * @brief Queue a scene to be loaded upon completion of the current frame.
         */
        virtual void QueueSceneChange(std::unique_ptr<Scene> scene) = 0;

        /**
         * @brief Check if a scene change is scheduled.
         */
        virtual bool IsSceneChangeQueued() const noexcept = 0;

        /**
         * @brief Get a pointer to the active registry.
         */
        virtual auto GetRegistry() noexcept -> Registry* = 0;

        /**
         * @brief Get a pointer to the module registry.
         */
        virtual auto GetModuleRegistry() noexcept -> ModuleRegistry* = 0;

        /**
         * @brief Compose a new task graph from all registered modules.
         * 
         * A graph is built on engine initialization. It only needs rebuilding
         * if modules are added afterwards. Only call this from the main thread
         * when the graph is not currently running (prior to calling Start() or
         * from a Scene).
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
