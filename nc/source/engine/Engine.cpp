#include "Engine.h"
#include "Core.h"
#include "debug/Utils.h"
#include "debug/Profiler.h"
#include "MainCamera.h"
#include "camera/MainCameraInternal.h"
#include "config/Config.h"
#include "config/ConfigInternal.h"
#include "input/InputInternal.h"
#include "graphics/Renderer.h"
#include "graphics/resources/ResourceManager.h"
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
    Engine::Engine(HINSTANCE hInstance)
        : m_isRunning{ false },
          m_frameDeltaTimeFactor{ 1.0f },
          m_jobSystem{2},
          m_window{ hInstance },
          m_graphics{ m_window.GetHWND(), m_window.GetHINSTANCE(), m_window.GetDimensions() },
          m_renderer{&m_graphics},
          m_ecs{&m_graphics, config::GetMemorySettings()},
          m_physics{m_ecs.GetRegistry(), &m_graphics, &m_jobSystem},
          m_sceneSystem{},
          m_time{},
          m_assetManager{},
          m_ui{m_window.GetHWND(), &m_graphics}
    {
        m_graphics.SetRenderer(&m_renderer);
        SetBindings();
        V_LOG("Engine initialized");
    }

    void Engine::DisableRunningFlag()
    {
        m_isRunning = false;
    }

    void Engine::MainLoop(std::unique_ptr<scene::Scene> initialScene)
    {
        V_LOG("Starting engine loop");
        m_sceneSystem.QueueSceneChange(std::move(initialScene));
        m_sceneSystem.DoSceneChange(m_ecs.GetRegistry());
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

        m_graphics.Clear();
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
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        m_graphics.FrameBegin();
        m_ui.FrameBegin();

        auto camViewMatrix = camera::CalculateViewMatrix();
        m_graphics.SetViewMatrix(camViewMatrix);
        
        auto cameraPos = camera::GetMainCameraTransform()->GetPosition();
        m_graphics.SetCameraPosition(cameraPos);

        #ifdef NC_EDITOR_ENABLED
        m_ui.Frame(&m_frameDeltaTimeFactor, m_ecs.GetRegistry());
        #else
        m_ui.Frame();
        #endif

        m_ui.FrameEnd();

        m_ecs.GetPointLightSystem()->Update();
        m_ecs.GetMeshRendererSystem()->Update();

        auto* renderer = m_graphics.GetRendererPtr();
        
        #ifdef NC_EDITOR_ENABLED
        auto* registry = m_ecs.GetRegistry();

        for(auto& collider : registry->ViewAll<Collider>())
            collider.UpdateWidget(renderer);
        #endif

        // @todo: conditionally update based on changes
        renderer->Record(m_graphics.GetCommandsPtr());

        m_graphics.Draw();
        m_graphics.FrameEnd();
        NC_PROFILE_END();
    }

    void Engine::FrameCleanup()
    {
        if(m_sceneSystem.IsSceneChangeScheduled())
        {
            DoSceneSwap();
        }
        
        input::Flush();
        m_time.ResetFrameDeltaTime();
        m_time.ResetFixedDeltaTime();
        m_time.UpdateTime();
    }

    void Engine::SetBindings()
    {
        using namespace std::placeholders;

        m_window.BindGraphicsOnResizeCallback(std::bind(graphics::Graphics::OnResize, &m_graphics, _1, _2, _3, _4, _5));
        m_window.BindGraphicsSetClearColorCallback(std::bind(graphics::Graphics::SetClearColor, &m_graphics, _1));
        m_window.BindUICallback(std::bind(ui::UIImpl::WndProc, &m_ui, _1, _2, _3, _4));
    }
} // end namespace nc::engine