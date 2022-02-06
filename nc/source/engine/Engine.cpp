#include "Engine.h"
#include "config/Config.h"
#include "config/ConfigInternal.h"
#include "debug/Utils.h"
#include "ecs/component/Logic.h"
#include "input/InputInternal.h"
#include "graphics/PerFrameRenderState.h"
#include "optick/optick.h"
#include "physics/PhysicsConstants.h"

namespace nc
{
    auto InitializeNcEngine(std::string_view configPath, bool useEditorMode) -> std::unique_ptr<NcEngine>
    {
        config::LoadInternal(configPath);
        debug::internal::OpenLog(config::GetProjectSettings().logFilePath);
        V_LOG("Constructing Engine Instance");
        return std::make_unique<Engine>(useEditorMode);
    }

    /* Engine */
    Engine::Engine(bool useEditorMode)
        : m_mainCamera{},
          m_window{},
          m_graphics{ &m_mainCamera, m_window.GetHWND(), m_window.GetHINSTANCE(), m_window.GetDimensions() },
          m_ecs{&m_graphics, config::GetMemorySettings()},
          m_physicsSystem{m_ecs.GetRegistry()},
          m_sceneSystem{},
          m_time{},
          m_audioSystem{m_ecs.GetRegistry()},
          m_environment{},
          m_uiSystem{m_window.GetHWND(), &m_graphics},
          m_taskExecutor{8u}, // @todo probably add to config
          m_tasks{},
          m_dt{0.0f},
          m_frameDeltaTimeFactor{1.0f},
          m_useEditorMode{useEditorMode},
          m_isRunning{false}
    {
        SetBindings();
        BuildTaskGraph();
        V_LOG("Engine constructed");
    }

    Engine::~Engine() noexcept
    {
        debug::internal::CloseLog();
    }

    void Engine::Start(std::unique_ptr<Scene> initialScene)
    {
        V_LOG("Starting NcEngine");
        m_ecs.GetRegistry()->VerifyCallbacks();
        m_sceneSystem.ChangeScene(std::move(initialScene));
        m_sceneSystem.DoSceneChange(this);
        m_isRunning = true;

        if(!m_useEditorMode)
            MainLoop();
        else
            EditorLoop();
    }

    void Engine::Quit() noexcept
    {
        V_LOG("Quit NcEngine");
        DisableRunningFlag();
    }

    void Engine::Shutdown() noexcept
    {
        V_LOG("Shutdown NcEngine");
        try
        {
            ClearState();
        }
        catch(const std::exception& e)
        {
            debug::LogException(e);
        }
    }

    auto Engine::Audio()       noexcept -> AudioSystem*     { return &m_audioSystem;      }
    auto Engine::Environment() noexcept -> nc::Environment* { return &m_environment;      }
    auto Engine::Registry()    noexcept -> nc::Registry*    { return m_ecs.GetRegistry(); }
    auto Engine::MainCamera()  noexcept -> nc::MainCamera*  { return &m_mainCamera;       }
    auto Engine::Physics()     noexcept -> PhysicsSystem*   { return &m_physicsSystem;    }
    auto Engine::SceneSystem() noexcept -> nc::SceneSystem* { return &m_sceneSystem;      }
    auto Engine::UI()          noexcept -> UISystem*        { return &m_uiSystem;         }

    void Engine::BuildTaskGraph()
    {
        V_LOG("Building Task Graph");
        [[maybe_unused]] auto writeAudioBuffersTask = m_tasks.AddGuardedTask(
            [&audioSystem = m_audioSystem]
        {
            OPTICK_CATEGORY("AudioSystem::Update", Optick::Category::Audio);
            audioSystem.Update();
        });

        [[maybe_unused]] auto particleSystemTask = m_tasks.AddGuardedTask(
            [particleEmitterSystem = m_ecs.GetParticleEmitterSystem(),
             &dt = m_dt]
        {
            particleEmitterSystem->UpdateParticles(dt);
        });

        #if NC_OUTPUT_TASKFLOW
        m_tasks.GetTaskFlow().name("Main loop");
        writeAudioBuffersTask.name("Write Audio Buffers");
        particleSystemTask.name("Update Particles");
        m_tasks.GetTaskFlow().dump(std::cout);
        #endif
    }

    void Engine::DisableRunningFlag() noexcept
    {
        m_isRunning = false;
    }

    void Engine::MainLoop()
    {
        V_LOG("Starting Game Loop");
        const auto fixedTimeStep = config::GetPhysicsSettings().fixedUpdateInterval;
        auto* particleEmitterSystem = m_ecs.GetParticleEmitterSystem();

        while(m_isRunning)
        {
            OPTICK_FRAME("Main Thread");

            m_dt = m_frameDeltaTimeFactor * m_time.UpdateTime();
            m_window.ProcessSystemMessages();
            auto mainLoopTasksResult = m_tasks.RunAsync(m_taskExecutor);
            RunFrameLogic(m_dt);

            size_t physicsIterations = 0u;
            while(physicsIterations < physics::MaxPhysicsIterations && m_time.GetAccumulatedTime() > fixedTimeStep)
            {
                /** @todo need to store prev transforms for interpolation */
                m_physicsSystem.DoPhysicsStep(m_taskExecutor);
                m_time.DecrementAccumulatedTime(fixedTimeStep);
                ++physicsIterations;
            }

            mainLoopTasksResult.wait();
            m_tasks.ThrowIfExceptionStored();
            m_ecs.GetRegistry()->CommitStagedChanges();
            FrameRender();
            particleEmitterSystem->ProcessFrameEvents();
            FrameCleanup();
        }

        Shutdown();
    }

    void Engine::EditorLoop()
    {
        V_LOG("Starting engine loop");
        auto* particleEmitterSystem = m_ecs.GetParticleEmitterSystem();

        while(m_isRunning)
        {
            m_dt = m_frameDeltaTimeFactor * static_cast<float>(m_time.UpdateTime());
            m_window.ProcessSystemMessages();
            auto mainLoopTasksResult = m_tasks.RunAsync(m_taskExecutor);
            RunFrameLogic(m_dt);

            mainLoopTasksResult.wait();
            m_tasks.ThrowIfExceptionStored();
            m_ecs.GetRegistry()->CommitStagedChanges();
            FrameRender();
            particleEmitterSystem->ProcessFrameEvents();
            FrameCleanup();
        }

        Shutdown();
    }

    void Engine::ClearState()
    {
        V_LOG("Clearing engine state");

        m_graphics.Clear();
        m_ecs.Clear();
        m_physicsSystem.ClearState();
        m_audioSystem.Clear();
        m_mainCamera.Set(nullptr);
        m_time.ResetFrameDeltaTime();
        m_time.ResetAccumulatedTime();
        m_environment.Clear();
        // SceneSystem state is never cleared
    }

    void Engine::DoSceneSwap()
    {
        V_LOG("Swapping scene");
        m_sceneSystem.UnloadActiveScene();
        ClearState();
        m_sceneSystem.DoSceneChange(this);
    }

    void Engine::RunFrameLogic(float dt)
    {
        OPTICK_CATEGORY("SendFrameUpdate", Optick::Category::GameLogic);
        auto* registry = m_ecs.GetRegistry();

        for(auto& frameLogic : registry->ViewAll<FrameLogic>())
            frameLogic.Run(registry, dt);
    }

    void Engine::FrameRender()
    {
        OPTICK_CATEGORY("FrameRender", Optick::Category::Rendering);
        /** Update the view matrix for the camera */
        auto* mainCamera = m_mainCamera.Get();
        mainCamera->UpdateViewMatrix();

        /** Setup the frame */
        if (m_graphics.FrameBegin() == UINT32_MAX) return;
        m_uiSystem.FrameBegin();

        auto* registry = m_ecs.GetRegistry();

        #ifdef NC_EDITOR_ENABLED
        m_uiSystem.Draw(&m_frameDeltaTimeFactor, registry);
        #else
        m_uiSystem.Draw();
        #endif

        /** Get the frame data */
        auto state = graphics::PerFrameRenderState{registry, mainCamera, m_ecs.GetPointLightSystem()->CheckDirtyAndReset(), &m_environment};
        graphics::MapPerFrameRenderState(state);

        /** Draw the frame */
        m_graphics.Draw(state);

        #ifdef NC_EDITOR_ENABLED
        for(auto& collider : registry->ViewAll<Collider>()) collider.SetEditorSelection(false);
        #endif

        /** End the frame */
        m_graphics.FrameEnd();
    }

    void Engine::FrameCleanup()
    {
        if(m_sceneSystem.IsSceneChangeScheduled())
        {
            DoSceneSwap();
        }

        input::Flush();
    }

    void Engine::SetBindings()
    {
        using namespace std::placeholders;

        m_window.BindGraphicsOnResizeCallback(std::bind(&graphics::Graphics::OnResize, &m_graphics, _1, _2, _3, _4, _5));
        m_window.BindGraphicsSetClearColorCallback(std::bind(&graphics::Graphics::SetClearColor, &m_graphics, _1));
        m_window.BindUICallback(std::bind(&ui::UISystemImpl::WndProc, &m_uiSystem, _1, _2, _3, _4));
        m_window.BindEngineDisableRunningCallback(std::bind(&Engine::DisableRunningFlag, this));
    }
} // end namespace nc::engine