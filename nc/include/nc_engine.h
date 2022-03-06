#pragma once

#include "audio/audio_module.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "ecs/component/All.h"
#include "graphics/graphics_module.h"
#include "math/Random.h"
#include "physics/physics_module.h"
#include "Scene.h"
#include "ui/UISystem.h"

namespace nc
{
    using engine_init_flags = uint8_t;
    static constexpr uint8_t engine_init_flags_headless_mode   = 0b00000001;
    static constexpr uint8_t engine_init_flags_disable_physics = 0b00000010;

    /** Core engine object. */
    class nc_engine
    {
        public:
            virtual ~nc_engine() = default;

            virtual void start(std::unique_ptr<Scene> initialScene) = 0;
            virtual void stop() noexcept = 0;
            virtual void shutdown() noexcept = 0;

            virtual auto Audio() noexcept -> audio_module* = 0;
            virtual auto Graphics() noexcept -> graphics_module* = 0;
            virtual auto Physics() noexcept -> physics_module* = 0;
            virtual auto Random() noexcept -> nc::Random* = 0;
            virtual auto Registry() noexcept -> nc::Registry* = 0;
            virtual auto SceneSystem() noexcept -> nc::SceneSystem* = 0;
    };

    auto initialize_nc_engine(std::string_view configPath, engine_init_flags flags = 0) -> std::unique_ptr<nc_engine>;
}