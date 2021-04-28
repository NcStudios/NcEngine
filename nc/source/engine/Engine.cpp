#include "Engine.h"
#include "Core.h"
#include "debug/Utils.h"
#include "debug/Profiler.h"
#include "MainCamera.h"
#include "camera/MainCameraInternal.h"
#include "config/Config.h"
#include "config/ConfigInternal.h"
#include "input/InputInternal.h"
#include "Ecs.h"

namespace nc::core
{
    /* Api Function Implementation */
    namespace internal
    {
        std::unique_ptr<Engine> impl = nullptr;
    }

    void Initialize(HINSTANCE hInstance)
    {
        IF_THROW(internal::impl != nullptr, "core::Initialize - Attempt to reinitialize engine");
        config::Load();
        debug::internal::OpenLog(config::GetProjectSettings().logFilePath);
        internal::impl = std::make_unique<Engine>(hInstance);
    }

    void Start(std::unique_ptr<scene::Scene> initialScene)
    {
        V_LOG("Starting engine");
        IF_THROW(internal::impl == nullptr, "core::Start - Engine is not initialized");
        internal::impl->MainLoop(std::move(initialScene));
    }

    void Quit(bool forceImmediate) noexcept
    {
        V_LOG("Shutting down engine - forceImmediate=" + std::to_string(forceImmediate));
        if(internal::impl)
        {
            internal::impl->DisableRunningFlag();
            if (forceImmediate)
                internal::impl->Shutdown();
        }
    }

    void Shutdown() noexcept
    {
        if(!internal::impl)
            return;

        internal::impl = nullptr;

        try
        {
            config::Save();
        }
        catch(const std::runtime_error& e)
        {
            debug::LogException(e);
        }

        debug::internal::CloseLog();
    }

    /* Engine */
    #ifdef USE_VULKAN
    Engine::Engine(HINSTANCE hInstance)
        : m_isRunning{ false },
          m_frameDeltaTimeFactor{ 1.0f },
          m_jobSystem{2},
          m_window{ hInstance },
          m_graphics2{ m_window.GetHWND(), m_window.GetHINSTANCE(), m_window.GetDimensions() },
          m_ecs{},
          m_physics{ &m_graphics2, m_ecs.GetColliderSystem(), &m_jobSystem},
          m_sceneSystem{},
          m_time{}
    {
        SetBindings();
        V_LOG("Engine initialized");
    }
    #else
    Engine::Engine(HINSTANCE hInstance)
        : m_isRunning{ false },
          m_frameDeltaTimeFactor{ 1.0f },
          m_jobSystem{2},
          m_window{ hInstance },
          m_graphics{ m_window.GetHWND(), m_window.GetDimensions() },
          m_pointLightManager{},
          m_frameManager{},
          m_ecs{},
          m_physics{&m_graphics, m_ecs.GetColliderSystem(), &m_jobSystem},
          m_sceneSystem{},
          m_time{},
          #ifdef NC_EDITOR_ENABLED
          m_ui{m_window.GetHWND(), &m_graphics, m_ecs.GetComponentSystems()}
          #else
          m_ui{m_window.GetHWND(), &m_graphics}
          #endif
    {
        SetBindings();
        V_LOG("Engine initialized");
    }
    #endif

    void Engine::DisableRunningFlag()
    {
        m_isRunning = false;
    }

    void Engine::MainLoop(std::unique_ptr<scene::Scene> initialScene)
    {
        V_LOG("Starting engine loop");
        m_sceneSystem.QueueSceneChange(std::move(initialScene));
        m_sceneSystem.DoSceneChange();
        auto fixedUpdateInterval = config::GetPhysicsSettings().fixedUpdateInterval;
        m_isRunning = true;
        
        while(m_isRunning)
        {
            m_time.UpdateTime();
            m_window.ProcessSystemMessages(); 

            if (m_time.GetFixedDeltaTime() > fixedUpdateInterval)
            {
                FixedStepLogic();
            }

            auto dt = m_time.GetFrameDeltaTime() * m_frameDeltaTimeFactor;
            FrameLogic(dt);
            FrameRender();
            FrameCleanup();
        }

        Shutdown();
    }

    void Engine::Shutdown()
    {
        V_LOG("Shutdown EngineImpl");
#ifdef USE_VULKAN
        // Block until all rendering is complete, so we do not tear down queues with pending operations.
        m_graphics2.WaitIdle();
#endif
        ClearState();
    }

    void Engine::ClearState()
    {
        V_LOG("Clearing engine state");
        m_ecs.ClearState();
        m_physics.ClearState();
        camera::ClearMainCamera();
        // SceneSystem state is never cleared
    }

    void Engine::DoSceneSwap()
    {
        V_LOG("Swapping scene");
        m_sceneSystem.UnloadActiveScene();
        ClearState();
        m_sceneSystem.DoSceneChange();
    }

    void Engine::FixedStepLogic()
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Physics);
        m_physics.DoPhysicsStep();
        m_ecs.SendFixedUpdate();
        m_time.ResetFixedDeltaTime();
        NC_PROFILE_END();
    }

    void Engine::FrameLogic(float dt)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Logic);
        m_ecs.SendFrameUpdate(dt);
        NC_PROFILE_END();
    }

    void Engine::FrameRender()
    {
#ifdef USE_VULKAN
        auto camViewMatrix = camera::GetMainCameraTransform()->GetViewMatrix();
        m_graphics2.SetViewMatrix(camViewMatrix);

        auto rendererSystem = m_ecs.GetRendererSystem();

        // @todo: conditionally update based on changes
        rendererSystem->RecordTechniques(m_graphics2.GetCommandsPtr());

        m_graphics2.Draw();
        m_graphics2.FrameEnd();
#else
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        m_ui.FrameBegin();
        m_graphics.FrameBegin();

        auto camViewMatrix = camera::GetMainCameraTransform()->GetViewMatrix();
        m_graphics.SetViewMatrix(camViewMatrix);

        for(auto& light : m_ecs.GetPointLightSystem()->GetComponents())
        {
            m_pointLightManager.AddPointLight(light.get(), camViewMatrix);
        }

        m_pointLightManager.Bind();

        for(auto& renderer : m_ecs.GetRendererSystem()->GetComponents())
        {
            renderer->Update(&m_frameManager);
        }

        #ifdef NC_EDITOR_ENABLED
        m_physics.UpdateWidgets(&m_frameManager);
        #endif

        m_frameManager.Execute(&m_graphics);

        #ifdef NC_EDITOR_ENABLED
        m_ui.Frame(&m_frameDeltaTimeFactor, m_ecs.GetActiveEntities());
        #else
        m_ui.Frame();
        #endif

        m_ui.FrameEnd();
        m_graphics.FrameEnd();
        NC_PROFILE_END();
#endif
    }

    void Engine::FrameCleanup()
    {
        m_ecs.SendOnDestroy();
        if(m_sceneSystem.IsSceneChangeScheduled())
        {
            DoSceneSwap();
        }
        input::Flush();
        #ifndef USE_VULKAN
            m_frameManager.Reset();
        #endif
        m_time.ResetFrameDeltaTime();
    }

    void Engine::SetBindings()
    {
        using namespace std::placeholders;

        #ifdef USE_VULKAN
            m_window.BindGraphicsOnResizeCallback(std::bind(graphics::Graphics2::OnResize, &m_graphics2, _1, _2, _3, _4, _5));
            // No UI for us until we tackle integrating IMGUI with Vulkan
        #else
            m_window.BindGraphicsOnResizeCallback(std::bind(graphics::Graphics::OnResize, &m_graphics, _1, _2, _3, _4, _5));
            m_window.BindUICallback(std::bind(ui::UIImpl::WndProc, &m_ui, _1, _2, _3, _4));
        #endif
    }
} // end namespace nc::engine