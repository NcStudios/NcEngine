#include "runtime.h"
#include "config/Config.h"

#include "audio/audio_module_impl.h"
#include "physics/physics_module_impl.h"
#include "graphics/PerFrameRenderState.h"
#include "input/InputInternal.h"

namespace
{
    auto build_modules(nc::window::WindowImpl* window, nc::engine_init_flags flags) -> nc::modules
    {
        V_LOG("build_modules()");
        bool enableApplicationModule = !(flags & nc::engine_init_flags_headless_mode);
        bool enablePhysicsModule = !(flags & nc::engine_init_flags_disable_physics);
        const auto& memorySettings = nc::config::GetMemorySettings();
        nc::modules out;
        out.registry = std::make_unique<nc::Registry>(memorySettings.maxTransforms);
        out.mainCamera = std::make_unique<nc::camera::MainCameraImpl>();
        attach_application(&out, window, out.mainCamera.get(), enableApplicationModule);
        out.particleSystem = std::make_unique<nc::ecs::ParticleEmitterSystem>(out.registry.get());
        out.pointLightSystem = std::make_unique<nc::ecs::PointLightSystem>(out.registry.get());

        out.physicsModule = nc::physics::build_physics_module(enablePhysicsModule, out.registry.get());
        out.sceneSystem = std::make_unique<nc::scene::SceneSystemImpl>();
        out.audioModule = nc::audio::build_audio_module(out.registry.get());

        out.environment = std::make_unique<nc::EnvironmentImpl>();
        out.random = std::make_unique<nc::Random>();
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
          m_modules{::build_modules(&m_window, flags)},
          m_time{},
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
        m_modules.registry->VerifyCallbacks();
        m_modules.sceneSystem->ChangeScene(std::move(initialScene));
        m_modules.sceneSystem->DoSceneChange(this);
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

    auto runtime::Audio() noexcept -> nc::audio_module*      { return m_modules.audioModule.get();     }
    auto runtime::Environment() noexcept -> nc::Environment* { return m_modules.environment.get();     }
    auto runtime::MainCamera() noexcept -> nc::MainCamera*   { return m_modules.mainCamera.get();      }
    auto runtime::Physics() noexcept -> physics_module*      { return m_modules.physicsModule.get();   }
    auto runtime::Random() noexcept -> nc::Random*           { return m_modules.random.get();          }
    auto runtime::Registry() noexcept -> nc::Registry*       { return m_modules.registry.get();        }
    auto runtime::SceneSystem() noexcept -> nc::SceneSystem* { return m_modules.sceneSystem.get();     }
    auto runtime::UI() noexcept -> UISystem*                 { return m_modules.application->get_ui(); }

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

        auto renderTask = m_tasks.AddGuardedTask([this]
        {
            OPTICK_CATEGORY("FrameRender", Optick::Category::Rendering);
            m_modules.registry->CommitStagedChanges();
            frame_render();
        }).name("Commit Changes and Render");

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
                time = &m_time,
                fixedStep = config::GetPhysicsSettings().fixedUpdateInterval]
            {
                return (curIt < maxIt && time->GetAccumulatedTime() > fixedStep) ? 1 : 0;
            }).name("Condition: Do Physics Step");

            auto updatePhysicsCondition = engineTF.emplace(
                [&i = m_currentPhysicsIterations,
                time = &m_time,
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

        renderTask.succeed(writeAudioBuffersTask, particleSystemTask, physicsFinished);
        endFrameTask.succeed(renderTask);

        #if NC_OUTPUT_TASKFLOW
        m_tasks.GetTaskFlow().name("Main Loop");
        m_tasks.GetTaskFlow().dump(std::cout);
        #endif
    }

    void runtime::clear()
    {
        //::clear(&m_modules);

        if(m_modules.application) m_modules.application->clear();
        m_modules.registry->Clear();
        m_modules.particleSystem->Clear();
        m_modules.pointLightSystem->Clear();

        //if(mods->physicsSystem) mods->physicsSystem->ClearState();
        m_modules.physicsModule->clear();
        
        m_modules.audioModule->clear();
        m_modules.mainCamera->Set(nullptr);
        m_time.ResetFrameDeltaTime();
        m_time.ResetAccumulatedTime();
        m_modules.environment->Clear();
    }

    void runtime::run()
    {
        while(m_isRunning)
        {
            OPTICK_FRAME("Main Thread");
            m_dt = m_dtFactor * m_time.UpdateTime();
            m_currentPhysicsIterations = 0u;
            m_window.ProcessSystemMessages();
            m_tasks.Run(m_taskExecutor);
        }

        shutdown();
    }

    void runtime::do_scene_swap()
    {
        V_LOG("runtime::do_scene_swap()");
        m_modules.sceneSystem->UnloadActiveScene();
        clear();
        m_modules.sceneSystem->DoSceneChange(this);
    }

    void runtime::run_frame_logic()
    {
        auto* registry = m_modules.registry.get();

        for(auto& frameLogic : view<FrameLogic>{registry})
            frameLogic.Run(registry, m_dt);
    }

    void runtime::frame_render()
    {
        if(!m_modules.application) return;

        /** Update the view matrix for the camera */
        auto* mainCamera = m_modules.mainCamera->Get();
        mainCamera->UpdateViewMatrix();

        /** Setup the frame */
        if (!m_modules.application->frame_begin()) return;

        auto* registry = m_modules.registry.get();

        #ifdef NC_EDITOR_ENABLED
        if(auto* ui = m_modules.application->get_ui()) ui->Draw(&m_dtFactor, registry);
        #else
        if(auto* ui = m_modules.application->get_ui()) ui->Draw();
        #endif

        /** Get the frame data */
        auto state = graphics::PerFrameRenderState{registry, mainCamera, m_modules.pointLightSystem->CheckDirtyAndReset(), m_modules.environment.get()};
        graphics::MapPerFrameRenderState(state);

        /** Draw the frame */
        m_modules.application->draw(state);

        #ifdef NC_EDITOR_ENABLED
        for(auto& collider : view<Collider>{registry}) collider.SetEditorSelection(false);
        #endif

        /** End the frame */
        m_modules.application->frame_end();
    }

    void runtime::frame_cleanup()
    {
        if(m_modules.sceneSystem->IsSceneChangeScheduled())
        {
            do_scene_swap();
        }

        input::Flush();
    }
}