#include "runtime.h"
#include "audio/audio_module_impl.h"
#include "config/ConfigInternal.h"
#include "graphics/graphics_module_impl.h"
#include "graphics/PerFrameRenderState.h"
#include "input/InputInternal.h"
#include "physics/physics_module_impl.h"

namespace
{
    auto build_context() -> nc::context
    {
        return nc::context
        {
            .registry = nc::Registry{nc::config::GetMemorySettings().maxTransforms},
            .time = nc::time::Time{},
            .scene = nc::scene::SceneSystemImpl{},
            .random = nc::Random{}
        };
    }

    auto build_modules(nc::Registry* reg, nc::window::WindowImpl* window, nc::engine_init_flags flags) -> nc::modules
    {
        V_LOG("build_modules()");
        bool enableGraphicsModule = !(flags & nc::engine_init_flags_headless_mode);
        bool enablePhysicsModule = !(flags & nc::engine_init_flags_disable_physics);
        nc::modules out;
        out.graphicsModule = nc::graphics::build_graphics_module(enableGraphicsModule, reg, window);
        out.particleSystem = std::make_unique<nc::ecs::ParticleEmitterSystem>(reg);
        out.physicsModule = nc::physics::build_physics_module(enablePhysicsModule, reg);
        out.audioModule = nc::audio::build_audio_module(reg);
        return out;
    }
}

namespace nc
{
    auto initialize_nc_engine(std::string_view configPath, engine_init_flags flags) -> std::unique_ptr<nc_engine>
    {
        config::LoadInternal(configPath);
        debug::internal::OpenLog(config::GetProjectSettings().logFilePath);
        V_LOG("initializing runtime");
        return std::make_unique<runtime>(flags);
    }

    runtime::runtime(engine_init_flags flags)
        : m_window{},
          m_context{::build_context()},
          m_modules{::build_modules(&m_context.registry, &m_window, flags)},
          m_taskExecutor{8},
          m_tasks{},
          m_dt{0.0f},
          m_dtFactor{1.0f},
          m_isRunning{false},
          m_currentPhysicsIterations{0u}
    {
        m_window.BindEngineDisableRunningCallback(std::bind_front(&runtime::stop, this));

        build_task_graph();
        V_LOG("runtime initialized");
    }

    void runtime::start(std::unique_ptr<Scene> initialScene)
    {
        V_LOG("runtime::start()");
        m_context.registry.VerifyCallbacks();
        m_context.scene.ChangeScene(std::move(initialScene));
        m_context.scene.DoSceneChange(this);
        m_isRunning = true;
        run();
    }

    void runtime::stop() noexcept
    {
        V_LOG("runtime::stop()");
        m_isRunning = false;
    }

    void runtime::shutdown() noexcept
    {
        V_LOG("runtime::shutdown()");
        try
        {
            clear();
        }
        catch(const std::exception& e)
        {
            debug::LogException(e);
        }
    }

    auto runtime::Audio() noexcept -> nc::audio_module*      { return m_modules.audioModule.get();    }
    auto runtime::Graphics() noexcept -> graphics_module*    { return m_modules.graphicsModule.get(); }
    auto runtime::Physics() noexcept -> physics_module*      { return m_modules.physicsModule.get();  }
    auto runtime::Random() noexcept -> nc::Random*           { return &m_context.random;              }
    auto runtime::Registry() noexcept -> nc::Registry*       { return &m_context.registry;            }
    auto runtime::SceneSystem() noexcept -> nc::SceneSystem* { return &m_context.scene;               }

    void runtime::build_task_graph()
    {
        V_LOG("runtime::build_task_graph()");
        auto frameLogicTask = m_tasks.AddGuardedTask([this]
        {
            OPTICK_CATEGORY("Frame Logic", Optick::Category::GameLogic);
            run_frame_logic();
        }).name("Frame Logic");

        auto writeAudioBuffersTask = m_tasks.AddGuardedTask([audioSystem = m_modules.audioModule.get()]
        {
            OPTICK_CATEGORY("AudioSystem::Update", Optick::Category::Audio);
            audioSystem->update();
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
            graphics->run(reg);
        }).name("Render");

        auto endFrameTask = m_tasks.AddGuardedTask([this]
        {
            m_modules.particleSystem->ProcessFrameEvents();
            frame_cleanup();
        }).name("Frame Cleanup");

        auto& engineTF = m_tasks.GetTaskFlow();
        auto physicsFinished = engineTF.emplace([]{}).name("Physics Finished");

        if(m_modules.physicsModule)
        {
            auto& physicsTF = m_modules.physicsModule->get_tasks().GetTaskFlow();
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
        }

        syncTask.succeed(writeAudioBuffersTask, particleSystemTask, physicsFinished);
        renderTask.succeed(syncTask);
        endFrameTask.succeed(renderTask);

        #if NC_OUTPUT_TASKFLOW
        m_tasks.GetTaskFlow().name("Main Loop");
        m_tasks.GetTaskFlow().dump(std::cout);
        #endif
    }

    void runtime::clear()
    {
        m_modules.graphicsModule->clear();
        m_context.registry.Clear();
        m_modules.particleSystem->Clear();
        m_modules.physicsModule->clear();
        m_modules.audioModule->clear();
        m_context.time.ResetFrameDeltaTime();
        m_context.time.ResetAccumulatedTime();
    }

    void runtime::run()
    {
        while(m_isRunning)
        {
            OPTICK_FRAME("Main Thread");
            m_dt = m_dtFactor * m_context.time.UpdateTime();
            m_currentPhysicsIterations = 0u;
            m_window.ProcessSystemMessages();
            m_tasks.Run(m_taskExecutor);
        }

        shutdown();
    }

    void runtime::do_scene_swap()
    {
        V_LOG("runtime::do_scene_swap()");
        m_context.scene.UnloadActiveScene();
        clear();
        m_context.scene.DoSceneChange(this);
    }

    void runtime::run_frame_logic()
    {
        auto* registry = &m_context.registry;

        for(auto& frameLogic : view<FrameLogic>{registry})
            frameLogic.Run(registry, m_dt);
    }

    void runtime::frame_cleanup()
    {
        if(m_context.scene.IsSceneChangeScheduled())
        {
            do_scene_swap();
        }

        input::Flush();
    }
}