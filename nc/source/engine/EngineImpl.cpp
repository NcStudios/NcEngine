#include "EngineImpl.h"
#include "DebugUtils.h"
#include "config/Config.h"
#include "graphics/Graphics.h"
#include "input/Input.h"
#include "time/NcTime.h"
#include "ApiBinder.h"
#include "camera/MainCameraImpl.h"
#include "debug/LogImpl.h"
#include "scene/SceneManagerImpl.h"
#include "ui/UIImpl.h"
#include "window/WindowImpl.h"
#include "Ecs.h"
#include "ecs/EcsImpl.h"
#include "physics/PhysicsSystem.h"
#include "component/PointLightManager.h"
#include "time/NcTime.h"
#include "Window.h"
#include "window/WindowImpl.h"
#include "graphics/rendergraph/FrameManager.h"

namespace nc::engine
{
    EngineImpl::EngineImpl(HINSTANCE hInstance, std::function<void(bool)> engineShutdownCallback)
    {
        isRunning = false;
        m_frameDeltaTimeFactor = 1.0f;
        m_log = std::make_unique<debug::LogImpl>(m_config.project.logFilePath);
        m_window = std::make_unique<window::WindowImpl>(hInstance, m_config, engineShutdownCallback);
        auto [width, height] = m_window->GetDimensions();
        auto hwnd = m_window->GetHWND();
        m_graphics = std::make_unique<graphics::Graphics>
        (
            hwnd, width, height,
            m_config.graphics.nearClip,
            m_config.graphics.farClip,
            m_config.graphics.launchInFullscreen
        );
        m_physics = std::make_unique<physics::PhysicsSystem>(m_graphics.get());
        m_ecs = std::make_unique<ecs::EcsImpl>();
        m_pointLightManager = std::make_unique<PointLightManager>();
        m_ui = std::make_unique<ui::UIImpl>(hwnd, m_graphics.get());
        m_sceneManager = std::make_unique<scene::SceneManagerImpl>();
        m_mainCamera = std::make_unique<camera::MainCameraImpl>();
        m_frameManager = std::make_unique<graphics::FrameManager>();
        #ifdef NC_EDITOR_ENABLED
        m_frameLogicTimer = std::make_unique<nc::time::Timer>();
        #endif

        SetBindings();
        V_LOG("Engine initialized");
    }

    EngineImpl::~EngineImpl()
    {
    }

    void EngineImpl::MainLoop(std::unique_ptr<scene::Scene> initialScene)
    {
        V_LOG("Starting engine loop");
        time::Time ncTime;
        m_sceneManager->QueueSceneChange(std::move(initialScene));
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
        m_ui->FrameBegin();
        m_graphics->FrameBegin();

        auto camMatrix = m_mainCamera->GetTransform()->CamGetMatrix();
        m_graphics->SetCamera(camMatrix);

        auto pointLightManager = m_pointLightManager.get();
        m_ecs->GetSystem<PointLight>()->ForEach([&camMatrix, pointLightManager](auto& light)
        {
            pointLightManager->AddPointLight(light, camMatrix);
        });
        m_pointLightManager->Bind();

        auto frameManager = m_frameManager.get();
        m_frameManager->Reset();

        m_ecs->GetSystem<Renderer>()->ForEach([frameManager](auto& renderer)
        {
            renderer.Update(*frameManager);
        });

        m_ecs->GetSystem<Collider>()->ForEach([frameManager](auto& collider)
        {
            collider.Update(*frameManager);
        });

        m_frameManager->Execute(m_graphics.get());

        #ifdef NC_EDITOR_ENABLED
        m_ui->Frame(&m_frameDeltaTimeFactor, m_frameLogicTimer->Value(), m_ecs->GetActiveEntities());
        #else
        m_ui->Frame();
        #endif
        m_ui->FrameEnd();
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

    const config::Config& EngineImpl::GetConfig() const
    {
        return m_config;
    }

    void EngineImpl::SetBindings()
    {
        m_window->BindGraphicsOnResizeCallback(std::bind(m_graphics->OnResize,
                                                         m_graphics.get(),
                                                         std::placeholders::_1,
                                                         std::placeholders::_2,
                                                         std::placeholders::_3,
                                                         std::placeholders::_4));

        m_window->BindUICallback(std::bind(m_ui->WndProc,
                                           m_ui.get(),
                                           std::placeholders::_1,
                                           std::placeholders::_2,
                                           std::placeholders::_3,
                                           std::placeholders::_4));

        ApiBinder::Bind(m_window.get(),
                        m_sceneManager.get(),
                        m_mainCamera.get(),
                        m_log.get(),
                        m_ui.get(),
                        this,
                        m_physics.get());

        Ecs::RegisterImpl(m_ecs.get());
    }
} // end namespace nc::engine