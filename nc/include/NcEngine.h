#pragma once

#include "audio/AudioModule.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "ecs/component/All.h"
#include "graphics/GraphicsModule.h"
#include "math/Random.h"
#include "physics/PhysicsModule.h"
#include "scene/SceneModule.h"
#include "utility/EnumUtilities.h"

namespace nc
{
    /** @brief Core engine object. */
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
             * @brief Get a pointer to the active registry.
             * @return Registry*
             */
            virtual auto Registry() noexcept -> nc::Registry* = 0;

            /**
             * @brief Get a pointer to the global random instance.
             * @return Random*
             */
            virtual auto Random() noexcept -> nc::Random* = 0;

            /**
             * @brief Get a pointer to the audio module.
             * @return AudioModule*
             */
            virtual auto Audio() noexcept -> AudioModule* = 0;

            /**
             * @brief Get a pointer to the graphics module.
             * @return GraphicsModule*
             */
            virtual auto Graphics() noexcept -> GraphicsModule* = 0;

            /**
             * @brief Get a pointer to the physics module.
             * @return PhysicsModule*
             */
            virtual auto Physics() noexcept -> PhysicsModule* = 0;

            /**
             * @brief Get a pointer to the scene module.
             * @return SceneModule*
             */
            virtual auto Scene() noexcept -> SceneModule* = 0;
    };

    /** @brief Flags controlling initialization of engine modules. */
    enum class EngineInitFlags : uint8_t
    {
        /** Standard initialization. */
        None = 0,

        /** Disable the graphics module. */
        Headless = 1,

        /** Disables the physics module. */
        NoPhysics = 1 << 1

    }; DEFINE_BITWISE_OPERATORS(EngineInitFlags)

    /**
     * @brief Create the engine runtime, modules, and context.
     * 
     * @param configPath Path to an ini file specifying configuration settings.
     * @param flags Flags for controlling initialization behavior.
     * @return std::unique_ptr<NcEngine>
     */
    auto InitializeNcEngine(std::string_view configPath, EngineInitFlags flags = EngineInitFlags::None) -> std::unique_ptr<NcEngine>;
}