#pragma once

#include "platform/win32/HInstanceForwardDecl.h"
#include "ecs/Registry.h"
#include "ecs/component/AudioSource.h"
#include "ecs/component/Camera.h"
#include "ecs/component/Collider.h"
#include "ecs/component/ConcaveCollider.h"
#include "ecs/component/MeshRenderer.h"
#include "ecs/component/ParticleEmitter.h"
#include "ecs/component/PhysicsBody.h"
#include "ecs/component/PointLight.h"
#include "ecs/component/Tag.h"
#include "ecs/component/Transform.h"
#include "MainCamera.h"
#include "AudioSystem.h"
#include "graphics/Environment.h"
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
            virtual auto Environment() noexcept -> Environment* = 0;
            virtual auto MainCamera() noexcept -> MainCamera* = 0;
            virtual auto Physics() noexcept -> PhysicsSystem* = 0;
            virtual auto Registry() noexcept -> Registry* = 0;
            virtual auto SceneSystem() noexcept -> SceneSystem* = 0;
            virtual auto UI() noexcept -> UISystem* = 0;
    };

    auto InitializeNcEngine(HINSTANCE hInstance, std::string_view configPath, bool useEditorMode = false) -> std::unique_ptr<NcEngine>;
}