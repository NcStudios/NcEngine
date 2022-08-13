#pragma once

#include "config/Config.h"
#include "ecs/Registry.h"
#include "ecs/component/All.h"
#include "module/ModuleRegistry.h"
#include "scene/Scene.h"
#include "time/Time.h"
#include "utility/EnumUtilities.h"

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
             * @brief Compose a new task graph from all registered modules. This is required
             *        only if module registration is changed after calling Start().
             */
            virtual void RebuildTaskGraph() = 0;
    };

    /** @brief Flags controlling initialization of engine modules. */
    enum class EngineInitFlags : uint8_t
    {
        /** Standard initialization. */
        None = 0,

        /** Disable the graphics module. */
        NoGraphics = 1,

        /** Disable the physics module. */
        NoPhysics = 1 << 1,

        /** Disable the audio module. */
        NoAudio = 1 << 2

    }; DEFINE_BITWISE_OPERATORS(EngineInitFlags)

    /**
     * @brief Create the engine runtime, context, and modules.
     * 
     * @param configPath Path to an ini file specifying configuration settings.
     * @param flags Flags for controlling initialization behavior.
     * @return std::unique_ptr<NcEngine>
     */
    auto InitializeNcEngine(std::string_view configPath, EngineInitFlags flags = EngineInitFlags::None) -> std::unique_ptr<NcEngine>;
}