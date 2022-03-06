#include "runtime.h"
#include "audio/AudioModuleImpl.h"
#include "config/ConfigInternal.h"
#include "graphics/GraphicsModuleImpl.h"
#include "graphics/PerFrameRenderState.h"
#include "input/InputInternal.h"
#include "physics/PhysicsModuleImpl.h"
#include "scene/SceneModuleImpl.h"

namespace
{
    auto BuildContext() -> nc::Context
    {
        V_LOG("BuildContext()");
        return nc::Context
        {
            .registry = nc::Registry{nc::config::GetMemorySettings().maxTransforms},
            .time = nc::time::Time{},
            .random = nc::Random{}
        };
    }

    auto BuildModules(nc::Registry* reg, nc::window::WindowImpl* window, std::function<void()> clearCallback, nc::engine_init_flags flags) -> nc::Modules
    {
        V_LOG("BuildModules()");
        bool enableGraphicsModule = !(flags & nc::engine_init_flags_headless_mode);
        bool enablePhysicsModule = !(flags & nc::engine_init_flags_disable_physics);
        return nc::Modules
        {
            .graphicsModule = nc::graphics::BuildGraphicsModule(enableGraphicsModule, reg, window),
            .particleSystem = std::make_unique<nc::ecs::ParticleEmitterSystem>(reg),
            .physicsModule = nc::physics::BuildPhysicsModule(enablePhysicsModule, reg),
            .audioModule = nc::audio::BuildAudioModule(reg),
            .sceneModule = std::make_unique<nc::scene::SceneModuleImpl>(std::move(clearCallback))
        };
    }
}

namespace nc
{
    auto InitializeNcEngine(std::string_view configPath, engine_init_flags flags) -> std::unique_ptr<NcEngine>
    {
        config::LoadInternal(configPath);
        debug::internal::OpenLog(config::GetProjectSettings().logFilePath);
        V_LOG("Initializing Runtime");
        return std::make_unique<Runtime>(flags);
    }

    Runtime::Runtime(engine_init_flags flags)
        : m_window{},
          m_context{BuildContext()},
          m_modules{BuildModules(&m_context.registry, &m_window, std::bind_front(&Runtime::Clear, this), flags)},
          m_taskExecutor{8},
          m_tasks{},
          m_dt{0.0f},
          m_dtFactor{1.0f},
          m_isRunning{false},
          m_currentPhysicsIterations{0u}
    {
        m_window.BindEngineDisableRunningCallback(std::bind_front(&Runtime::Stop, this));
        BuildTaskGraph();
        V_LOG("Runtime Initialized");
    }

    void Runtime::Start(std::unique_ptr<nc::Scene> initialScene)
    {
        V_LOG("Runtime::Start()");
        m_context.registry.VerifyCallbacks();
        m_modules.sceneModule->ChangeScene(std::move(initialScene));
        m_modules.sceneModule->DoSceneSwap(this);
        m_isRunning = true;
        Run();
    }

    void Runtime::Stop() noexcept
    {
        V_LOG("Runtime::Stop()");
        m_isRunning = false;
    }

    void Runtime::Shutdown() noexcept
    {
        V_LOG("Runtime::Shutdown()");
        try
        {
            Clear();
        }
        catch(const std::exception& e)
        {
            debug::LogException(e);
        }
    }

    auto Runtime::Audio()    noexcept -> AudioModule*    { return m_modules.audioModule.get();    }
    auto Runtime::Graphics() noexcept -> GraphicsModule* { return m_modules.graphicsModule.get(); }
    auto Runtime::Physics()  noexcept -> PhysicsModule*  { return m_modules.physicsModule.get();  }
    auto Runtime::Random()   noexcept -> nc::Random*     { return &m_context.random;              }
    auto Runtime::Registry() noexcept -> nc::Registry*   { return &m_context.registry;            }
    auto Runtime::Scene()    noexcept -> SceneModule*    { return m_modules.sceneModule.get();    }

    void Runtime::BuildTaskGraph()
    {
        V_LOG("Runtime::BuildTaskGraph()");
        auto frameLogicTask = m_tasks.AddGuardedTask([this]
        {
            OPTICK_CATEGORY("Frame Logic", Optick::Category::GameLogic);
            RunFrameLogic();
        }).name("Frame Logic");

        auto writeAudioBuffersTask = m_tasks.AddGuardedTask([audioSystem = m_modules.audioModule.get()]
        {
            OPTICK_CATEGORY("AudioSystem::Update", Optick::Category::Audio);
            audioSystem->Update();
        }).name("Write Audio Buffers");

        auto particleSystemTask = m_tasks.AddGuardedTask(
            [particleEmitterSystem = m_modules.particleSystem.get(), &dt = m_dt]
        {
            OPTICK_CATEGORY("Update Particles", Optick::Category::VFX);
            particleEmitterSystem->UpdateParticles(dt);
        }).name("Update Particles");

        auto syncTask = m_tasks.AddGuardedTask([reg = &m_context.registry]
        {
            OPTICK_CATEGORY("Sync State", Optick::Category::None);
            reg->CommitStagedChanges();
        }).name("Synchronize and Commit Deltas");

        auto renderTask = m_tasks.AddGuardedTask(
            [reg = &m_context.registry,
             graphics = m_modules.graphicsModule.get()]
        {
            OPTICK_CATEGORY("Render", Optick::Category::Rendering);
            graphics->Run(reg);
        }).name("Render");

        auto endFrameTask = m_tasks.AddGuardedTask(
            [particle = m_modules.particleSystem.get(),
             scene = m_modules.sceneModule.get(),
             engine = this]
        {
            particle->ProcessFrameEvents();
            scene->DoSceneSwap(engine);
        }).name("End Frame");

        auto& engineTF = m_tasks.GetTaskFlow();
        auto physicsFinished = engineTF.emplace([]{}).name("Physics Finished");

        /** @todo move inside physics */
        auto& physicsTF = m_modules.physicsModule->GetTasks().GetTaskFlow();
        auto physicsModule = engineTF.composed_of(physicsTF).name("Physics Module");

        auto physicsStepCondition = engineTF.emplace(
            [&curIt = m_currentPhysicsIterations,
            maxIt = physics::MaxPhysicsIterations,
            time = &m_context.time,
            fixedStep = config::GetPhysicsSettings().fixedUpdateInterval]
        {
            return (curIt < maxIt && time->GetAccumulatedTime() > fixedStep) ? 1 : 0;
        }).name("Condition: Do Physics Step");

        auto updatePhysicsCondition = engineTF.emplace(
            [&i = m_currentPhysicsIterations,
            time = &m_context.time,
            fixedStep = config::GetPhysicsSettings().fixedUpdateInterval]
        {
            time->DecrementAccumulatedTime(fixedStep);
            ++i;
            return 0;
        }).name("Update Accumulated Time");

        physicsStepCondition.succeed(frameLogicTask);
        physicsStepCondition.precede(physicsFinished, physicsModule);
        physicsModule.precede(updatePhysicsCondition);
        updatePhysicsCondition.precede(physicsStepCondition);

        syncTask.succeed(writeAudioBuffersTask, particleSystemTask, physicsFinished);
        renderTask.succeed(syncTask);
        endFrameTask.succeed(renderTask);

        #if NC_OUTPUT_TASKFLOW
        m_tasks.GetTaskFlow().name("Main Loop");
        m_tasks.GetTaskFlow().dump(std::cout);
        #endif
    }

    void Runtime::Clear()
    {
        m_modules.graphicsModule->Clear();
        m_context.registry.Clear();
        m_modules.particleSystem->Clear();
        m_modules.physicsModule->Clear();
        m_modules.audioModule->Clear();
        m_context.time.ResetFrameDeltaTime();
        m_context.time.ResetAccumulatedTime();
    }

    void Runtime::Run()
    {
        while(m_isRunning)
        {
            OPTICK_FRAME("Main Thread");
            m_dt = m_dtFactor * m_context.time.UpdateTime();
            m_currentPhysicsIterations = 0u;
            input::Flush();
            m_window.ProcessSystemMessages();
            m_tasks.Run(m_taskExecutor);
        }

        Shutdown();
    }

    void Runtime::RunFrameLogic()
    {
        auto* registry = &m_context.registry;

        for(auto& frameLogic : view<FrameLogic>{registry})
            frameLogic.Run(registry, m_dt);
    }
}