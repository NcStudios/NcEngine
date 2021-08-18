#include "Engine.h"
#include "Core.h"
#include "debug/Utils.h"
#include "debug/Profiler.h"
#include "MainCamera.h"
#include "camera/MainCameraInternal.h"
#include "config/Config.h"
#include "config/ConfigInternal.h"
#include "input/InputInternal.h"
#include "graphics/vulkan/Renderer.h"
#include "graphics/vulkan/resources/ResourceManager.h"
#include "graphics/Model.h" // @todo: Hack - remove when DX11 is removed.
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
          m_jobSystem{4},
          m_window{ hInstance },
          m_graphics2{ m_window.GetHWND(), m_window.GetHINSTANCE(), m_window.GetDimensions() },
          m_renderer{ &m_graphics2 },
          m_ecs{&m_graphics2, config::GetMemorySettings()},
          m_physics{ &m_graphics2, m_ecs.GetColliderSystem(), &m_jobSystem},
          m_sceneSystem{},
          m_time{},
          m_ui{m_window.GetHWND(), &m_graphics2}
    {
        m_graphics2.SetRenderer(&m_renderer);
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
          m_ecs{&m_graphics, config::GetMemorySettings()},
          m_physics{&m_graphics, &m_jobSystem},
          m_sceneSystem{},
          m_time{},
          m_ui{m_window.GetHWND(), &m_graphics}
    {
        SetBindings();
        m_ecs.GetRegistry()->VerifyCallbacks();
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
        m_sceneSystem.DoSceneChange(m_ecs.GetRegistry());
        /** @todo Enable interval and tinker a bit once kinematics are popped, locked, and dropped */
        //auto fixedUpdateInterval = config::GetPhysicsSettings().fixedUpdateInterval;
        m_isRunning = true;
        
        auto* particleEmitterSystem = m_ecs.GetParticleEmitterSystem();

        while(m_isRunning)
        {
            m_time.UpdateTime();
            m_window.ProcessSystemMessages(); 

            auto dt = m_time.GetFrameDeltaTime() * m_frameDeltaTimeFactor;

            auto particleUpdateJobResult = m_jobSystem.Schedule(ecs::ParticleEmitterSystem::UpdateParticles, particleEmitterSystem, dt);
            
            FrameLogic(dt);

            /** @todo see fixedUpdateInterval todo above */
            FixedStepLogic(dt);

            particleUpdateJobResult.wait();

            m_ecs.GetRegistry()->CommitStagedChanges();
            FrameRender();

            particleEmitterSystem->ProcessFrameEvents();

            FrameCleanup();
        }

        Shutdown();
    }

    void Engine::Shutdown()
    {
        V_LOG("Shutdown EngineImpl");
        ClearState();
    }

    void Engine::ClearState()
    {
        V_LOG("Clearing engine state");

#ifdef USE_VULKAN
        m_graphics2.Clear();
#endif

        m_ecs.Clear();
        m_physics.ClearState();
        camera::ClearMainCamera();
        // SceneSystem state is never cleared
    }

    void Engine::DoSceneSwap()
    {
        V_LOG("Swapping scene");
        m_sceneSystem.UnloadActiveScene();
        ClearState();
        m_sceneSystem.DoSceneChange(m_ecs.GetRegistry());
    }

    void Engine::FixedStepLogic(float dt)
    {
        m_physics.DoPhysicsStep(dt);

        for(auto& group : m_ecs.GetRegistry()->ViewAll<AutoComponentGroup>())
            group.SendFixedUpdate();

        m_time.ResetFixedDeltaTime();
    }

    void Engine::FrameLogic(float dt)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Logic);

        for(auto& group : m_ecs.GetRegistry()->ViewAll<AutoComponentGroup>())
            group.SendFrameUpdate(dt);
        
        NC_PROFILE_END();
    }

    void Engine::FrameRender()
    {
#ifdef USE_VULKAN
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        m_graphics2.FrameBegin();
        m_ui.FrameBegin();

        auto camViewMatrix = camera::CalculateViewMatrix();
        m_graphics2.SetViewMatrix(camViewMatrix);
        
        auto cameraPos = camera::GetMainCameraTransform()->GetPosition();
        m_graphics2.SetCameraPosition(cameraPos);

        #ifdef NC_EDITOR_ENABLED
        m_ui.Frame(&m_frameDeltaTimeFactor, m_ecs.GetRegistry());
        #else
        m_ui.Frame();
        #endif

        m_ui.FrameEnd();

        m_ecs.GetPointLightSystem()->Update();

        #ifdef NC_EDITOR_ENABLED
        auto* registry = m_ecs.GetRegistry();
        auto* renderer = m_graphics2.GetRendererPtr();

        for(auto& collider : registry->ViewAll<Collider>())
            collider.UpdateWidget(renderer);
        #endif

        // @todo: conditionally update based on changes
        renderer->Record(m_graphics2.GetCommandsPtr());

        m_graphics2.Draw();
        m_graphics2.FrameEnd();
        NC_PROFILE_END();
#else
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        m_ui.FrameBegin();
        m_graphics.FrameBegin();

        auto camViewMatrix = camera::CalculateViewMatrix();
        m_graphics.SetViewMatrix(camViewMatrix);

        auto* registry = m_ecs.GetRegistry();

        for(auto& light : registry->ViewAll<PointLight>())
            m_pointLightManager.AddPointLight(&light, camViewMatrix);

        m_pointLightManager.Bind();

        for(auto& renderer : registry->ViewAll<Renderer>())
            renderer.Update(&m_frameManager);

#ifdef NC_EDITOR_ENABLED
        for(auto& collider : registry->ViewAll<Collider>())
            collider.UpdateWidget(&m_frameManager);
#endif

        m_ecs.GetParticleEmitterSystem()->RenderParticles();

        m_frameManager.Execute(&m_graphics);

        #ifdef NC_DEBUG_RENDERING
        m_physics.DebugRender();
        #endif

        #ifdef NC_EDITOR_ENABLED
        m_ui.Frame(&m_frameDeltaTimeFactor, m_ecs.GetRegistry());
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
            m_window.BindGraphicsSetClearColorCallback(std::bind(graphics::Graphics2::SetClearColor, &m_graphics2, _1));
        #else
            m_window.BindGraphicsOnResizeCallback(std::bind(graphics::Graphics::OnResize, &m_graphics, _1, _2, _3, _4, _5));
            m_window.BindGraphicsSetClearColorCallback(std::bind(graphics::Graphics::SetClearColor, &m_graphics, _1));
        #endif
            m_window.BindUICallback(std::bind(ui::UIImpl::WndProc, &m_ui, _1, _2, _3, _4));
    }
} // end namespace nc::engine