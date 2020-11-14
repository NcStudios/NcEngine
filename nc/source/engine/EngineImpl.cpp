#include "EngineImpl.h"
#include "camera/MainCamera.h"
#include "config/Config.h"
#include "graphics/Graphics.h"
#include "input/Input.h"
#include "Ecs.h"
#include "ecs/EcsImpl.h"
#include "NcDebug.h"
#include "physics/PhysicsSystem.h"
#include "scene/SceneManager.h"
#include "time/NcTime.h"
#include "ui/UISystem.h"
#include "Window.h"
#include "window/WindowImpl.h"

#include <iostream>

namespace nc::engine
{
    EngineImpl::EngineImpl(HINSTANCE hInstance, Engine* topLevelEngine)
    {
        isRunning = false;
        m_config.Load();
        m_frameDeltaTimeFactor = 1.0f;
        m_logger = std::make_unique<log::Logger>(m_config.project.logFilePath);
        m_window = std::make_unique<window::WindowImpl>(hInstance, topLevelEngine, m_config);
        auto dim = m_window->GetDimensions();
        auto hwnd = m_window->GetHWND();
        m_graphics = std::make_unique<graphics::Graphics>
        (
            hwnd, dim.X(), dim.Y(),
            m_config.graphics.nearClip,
            m_config.graphics.farClip,
            m_config.graphics.launchInFullscreen
        );
        m_physics = std::make_unique<physics::PhysicsSystem>(m_graphics.get());
        m_ecs = std::make_unique<ecs::EcsImpl>();
        m_uiSystem = std::make_unique<ui::UISystem>(hwnd, m_graphics.get());
        m_sceneManager = std::make_unique<scene::SceneManager>();
        m_mainCamera = std::make_unique<camera::MainCamera>();
        #ifdef NC_EDITOR_ENABLED
        m_frameLogicTimer = std::make_unique<nc::time::Timer>();
        #endif

        m_window->BindGraphics(m_graphics.get());
        m_window->BindUISystem(m_uiSystem.get());
        Window::RegisterImpl(m_window.get());
        Ecs::RegisterImpl(m_ecs.get());

        V_LOG("Engine initialized");
    }

    EngineImpl::~EngineImpl()
    {
    }

    void EngineImpl::MainLoop(std::unique_ptr<scene::Scene> initialScene)
    {
        V_LOG("Starting engine loop");
        time::Time ncTime;
        m_sceneManager->QueueSceneChange_(std::move(initialScene));
        m_sceneManager->DoSceneChange();
        m_sceneManager->LoadActiveScene();
        isRunning = true;

        while(isRunning)
        {
            if(input::GetKey(input::KeyCode::R)) { m_graphics.get()->ResizeTarget(500, 500); }
            if(input::GetKey(input::KeyCode::F)) { m_graphics.get()->ToggleFullscreen(); }

            ncTime.UpdateTime();
            m_window->ProcessSystemMessages();

            /** @note Change this so physics 'simulates' running at a fixed interval.
             * It may need to run multiple times in a row in cases where FrameUpdate()
             * runs slowly and execution doesn't return back to physics in time for the 
             * next interval.
             */
            if (time::Time::FixedDeltaTime > m_config.physics.fixedUpdateInterval)
            {
                m_ecs->SendFixedUpdate();
                ncTime.ResetFixedDeltaTime();
            }

            auto dt = time::Time::FrameDeltaTime * m_frameDeltaTimeFactor;

            #ifdef NC_EDITOR_ENABLED
            m_frameLogicTimer->Start();
            #endif
            FrameLogic(dt);
            #ifdef NC_EDITOR_ENABLED
            m_frameLogicTimer->Stop();
            #endif
            FrameRender();
            FrameCleanup();
            ncTime.ResetFrameDeltaTime();
        }

        Shutdown();
    }

    void EngineImpl::Shutdown()
    {
        V_LOG("Shutdown EngineImpl");
        m_config.Save();
        ClearState();
    }

    void EngineImpl::ClearState()
    {
        V_LOG("Clearing engine state");
        m_ecs->ClearState();
        m_mainCamera->ClearTransform();
        // SceneManager state is never cleared
    }

    void EngineImpl::DoSceneSwap()
    {
        V_LOG("Swapping scene");
        m_sceneManager->UnloadActiveScene();
        ClearState();
        m_sceneManager->DoSceneChange();
        m_sceneManager->LoadActiveScene();
    }

    void EngineImpl::FrameLogic(float dt)
    {
        m_ecs->SendFrameUpdate(dt);
    }

    void EngineImpl::FrameRender()
    {
        m_uiSystem->FrameBegin();
        m_graphics->FrameBegin();

        auto camMatrix = m_mainCamera->GetTransform_()->CamGetMatrix();
        m_graphics->SetCamera(camMatrix);
        
        m_ecs->GetSystem<PointLight>()->ForEach([&camMatrix](auto& light)
        {
            light.Bind(camMatrix);
        });

        auto gfx = m_graphics.get();
        m_ecs->GetSystem<Renderer>()->ForEach([gfx](auto& renderer)
        {
            renderer.Update(gfx);
        });

        #ifdef NC_EDITOR_ENABLED
        m_uiSystem->Frame(&m_frameDeltaTimeFactor, m_frameLogicTimer->Value(), m_ecs->GetActiveEntities());
        #else
        m_uiSystem->Frame();
        #endif
        m_uiSystem->FrameEnd();
        m_graphics->FrameEnd();
    }

    void EngineImpl::FrameCleanup()
    {
        m_ecs->SendOnDestroy();
        if(m_sceneManager->IsSceneChangeScheduled())
        {
            DoSceneSwap();
        }
        input::Flush();
    }
} // end namespace nc::engine