#pragma once

#include "ecs/Registry.h"
#include "ecs/component/All.h"
#include "MainCamera.h"
#include "AudioSystem.h"
#include "graphics/Environment.h"
#include "math/Random.h"
#include "physics/PhysicsSystem.h"
#include "ui/UISystem.h"
#include "Scene.h"
#include "config/Config.h"

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
            virtual void Quit() noexcept = 0;
            virtual void Shutdown() noexcept = 0;

            virtual auto Audio() noexcept -> AudioSystem* = 0;
            virtual auto Environment() noexcept -> Environment* = 0;
            virtual auto MainCamera() noexcept -> MainCamera* = 0;
            virtual auto Physics() noexcept -> PhysicsSystem* = 0;
            virtual auto Random() noexcept -> Random* = 0;
            virtual auto Registry() noexcept -> Registry* = 0;
            virtual auto SceneSystem() noexcept -> SceneSystem* = 0;
            virtual auto UI() noexcept -> UISystem* = 0;
    };

    auto InitializeNcEngine(std::string_view configPath, engine_init_flags flags = 0) -> std::unique_ptr<NcEngine>;
}