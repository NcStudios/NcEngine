#include "Engine.h"
#include "config/Config.h"
#include "config/ConfigInternal.h"
#include "debug/Utils.h"
#include "debug/Profiler.h"
#include "input/InputInternal.h"
#include "graphics/Renderer.h"
#include "graphics/PerFrameRenderState.h"
#include "physics/PhysicsConstants.h"

namespace nc
{
    auto InitializeNcEngine(HINSTANCE hInstance, const std::string& configPath) -> std::unique_ptr<NcEngine>
    {
        config::Load(configPath);
        debug::internal::OpenLog(config::GetProjectSettings().logFilePath);
        V_LOG("Constructing Engine Instance");
        return std::make_unique<Engine>(hInstance);
    }

    /* Engine */
    Engine::Engine(HINSTANCE hInstance)
        : m_mainCamera{},
          m_window{ hInstance },
          m_graphics{ &m_mainCamera, m_window.GetHWND(), m_window.GetHINSTANCE(), m_window.GetDimensions(), &m_assetServices },
          m_assetServices{&m_graphics, config::GetMemorySettings().maxTextures},
          m_ecs{&m_graphics, config::GetMemorySettings()},
          m_physicsSystem{m_ecs.GetRegistry(), &m_graphics},
          m_sceneSystem{},
          m_time{},
          m_audioSystem{m_ecs.GetRegistry()},
          m_uiSystem{m_window.GetHWND(), &m_graphics},
          m_taskExecutor{6u}, // @todo probably add to config
          m_tasks{},
          m_dt{0.0f},
          m_frameDeltaTimeFactor{1.0f},
          m_currentImageIndex{0},
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
        MainLoop();
    }

    void Engine::Quit() noexcept
    {
        V_LOG("Quit NcEngine");
        DisableRunningFlag();
    }

    void Engine::Shutdown() noexcept
    {
        V_LOG("Shutdown NcEngine");
        ClearState();
    }

    auto Engine::Audio()       noexcept -> AudioSystem*     { return &m_audioSystem;      }
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

    void Engine::DisableRunningFlag()
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
            m_dt = m_frameDeltaTimeFactor * m_time.UpdateTime();
            m_window.ProcessSystemMessages();
            auto mainLoopTasksResult = m_tasks.RunAsync(m_taskExecutor);
            FrameLogic(m_dt);

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
        // SceneSystem state is never cleared
    }

    void Engine::DoSceneSwap()
    {
        V_LOG("Swapping scene");
        m_sceneSystem.UnloadActiveScene();
        ClearState();
        m_sceneSystem.DoSceneChange(this);
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
        auto* mainCamera = m_mainCamera.Get();
        mainCamera->UpdateViewMatrix();
        m_currentImageIndex = m_graphics.FrameBegin();

        if (m_currentImageIndex == UINT32_MAX)
        {
            return;
        }
        
        m_ui.FrameBegin();
        m_uiSystem.FrameBegin();
        auto* registry = m_ecs.GetRegistry();

        #ifdef NC_EDITOR_ENABLED
        m_uiSystem.Frame(&m_frameDeltaTimeFactor, registry);
        #else
        m_uiSystem.Frame();
        #endif

        m_uiSystem.FrameEnd();

        auto state = graphics::PerFrameRenderState{registry, mainCamera, m_ecs.GetPointLightSystem()->CheckDirtyAndReset()};
        graphics::MapPerFrameRenderState(state);

        auto* renderer = m_graphics.GetRendererPtr();

        // @todo: conditionally update based on changes
        renderer->Record(m_graphics.GetCommandsPtr(), state, m_currentImageIndex);

        #ifdef NC_EDITOR_ENABLED
        for(auto& collider : registry->ViewAll<Collider>())
        {
            collider.SetEditorSelection(false);
        }
        #endif

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
    }

    void Engine::SetBindings()
    {
        using namespace std::placeholders;

        m_window.BindGraphicsOnResizeCallback(std::bind(graphics::Graphics::OnResize, &m_graphics, _1, _2, _3, _4, _5));
        m_window.BindGraphicsSetClearColorCallback(std::bind(graphics::Graphics::SetClearColor, &m_graphics, _1));
        m_window.BindUICallback(std::bind(ui::UISystemImpl::WndProc, &m_uiSystem, _1, _2, _3, _4));
        m_window.BindEngineDisableRunningCallback(std::bind(Engine::DisableRunningFlag, this));
    }
} // end namespace nc::engine