#pragma once

#include "platform/win32/HInstanceForwardDecl.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "AudioSystem.h"
#include "physics/PhysicsSystem.h"
#include "ui/UISystem.h"
#include "Scene.h"
#include "config/Config.h"

namespace nc
{
    /** Core engine object. */
    class NcEngine
    {
        public:
            virtual ~NcEngine() = default;

            virtual void Start(std::unique_ptr<Scene> initialScene) = 0;
            virtual void Quit() noexcept = 0;
            virtual void Shutdown() noexcept = 0;

            virtual auto Audio() noexcept -> AudioSystem* = 0;
            virtual auto MainCamera() noexcept -> MainCamera* = 0;
            virtual auto Physics() noexcept -> PhysicsSystem* = 0;
            virtual auto Registry() noexcept -> registry_type* = 0;
            virtual auto SceneSystem() noexcept -> SceneSystem* = 0;
            virtual auto UI() noexcept -> UISystem* = 0;
    };

    auto InitializeNcEngine(HINSTANCE hInstance, const std::string& configPath, bool useEditorMode = false) -> std::unique_ptr<NcEngine>;
}