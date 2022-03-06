#pragma once

#include "audio/AudioModule.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "ecs/component/All.h"
#include "graphics/GraphicsModule.h"
#include "math/Random.h"
#include "physics/PhysicsModule.h"
#include "Scene.h"

namespace nc
{
    using engine_init_flags = uint8_t;
    static constexpr uint8_t engine_init_flags_headless_mode   = 0b00000001;
    static constexpr uint8_t engine_init_flags_disable_physics = 0b00000010;

    /** Core engine object. */
    class NcEngine
    {
        public:
            virtual ~NcEngine() = default;

            virtual void Start(std::unique_ptr<Scene> initialScene) = 0;
            virtual void Stop() noexcept = 0;
            virtual void Shutdown() noexcept = 0;

            virtual auto Audio() noexcept -> AudioModule* = 0;
            virtual auto Graphics() noexcept -> GraphicsModule* = 0;
            virtual auto Physics() noexcept -> PhysicsModule* = 0;
            virtual auto Random() noexcept -> nc::Random* = 0;
            virtual auto Registry() noexcept -> nc::Registry* = 0;
            virtual auto SceneSystem() noexcept -> nc::SceneSystem* = 0;
    };

    auto InitializeNcEngine(std::string_view configPath, engine_init_flags flags = 0) -> std::unique_ptr<NcEngine>;
}