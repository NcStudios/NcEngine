#include "engine_executor.h"
#include "config/Config.h"
#include "ecs/component/logic.h"
#include "ecs/view.h"
#include "graphics/PerFrameRenderState.h"
#include "input/InputInternal.h"
#include "optick/optick.h"
#include "physics/PhysicsConstants.h"

namespace nc
{
    engine_executor::engine_executor(engine_context context, NcEngine* engine)
        : m_context{std::move(context)},
          m_taskExecutor{8u}, /** @todo add to config */
          m_tasks{},
          m_engine{engine},
          m_dt{0.0f},
          m_frameDeltaTimeFactor{1.0f},
          m_currentPhysicsIterations{0ull},
          m_isRunning{false}
    {
        if(m_context.application)
            m_context.application->register_shutdown_callback(std::bind_front(&engine_executor::stop, this));

        build_task_graph();
        V_LOG("engine_executor constructed");
    }

    void engine_executor::start(std::unique_ptr<Scene> initialScene)
    {
        V_LOG("engine_executor::start()");
        m_context.registry->VerifyCallbacks();
        m_context.sceneSystem->ChangeScene(std::move(initialScene));
        m_context.sceneSystem->DoSceneChange(m_engine);
        m_isRunning = true;
        game_loop();
    }

    void engine_executor::stop() noexcept
    {
        V_LOG("engine_executor::stop()");
        m_isRunning = false;
    }

    void engine_executor::shutdown() noexcept
    {
        V_LOG("engine_executor::shutdown()");
        try
        {
            clear();
        }
        catch(const std::exception& e)
        {
            debug::LogException(e);
        }
    }

    void engine_executor::game_loop()
    {
        V_LOG("engine_executor::game_loop()");

    #if 1
        while(m_isRunning)
        {
            OPTICK_FRAME("Main Thread");
            m_dt = m_frameDeltaTimeFactor * m_context.time->UpdateTime();
            m_currentPhysicsIterations = 0ull;

            if(m_context.application)
                m_context.application->process_system_messages();

            m_tasks.Run(m_taskExecutor);
        }

        shutdown();
    #else // force single threaded for debugging
        const auto fixedTimeStep = config::GetPhysicsSettings().fixedUpdateInterval;

        while(m_isRunning)
        {
            OPTICK_FRAME("Main Thread");
            auto* time = m_context.time.get();
            m_dt = m_frameDeltaTimeFactor * time->UpdateTime();
            if(m_context.application) m_context.application->process_system_messages();
            m_context.audioSystem->Update();
            m_context.particleSystem->UpdateParticles(m_dt);
            run_frame_logic(m_dt);

            if(auto* physics = m_context.physicsSystem.get())
            {
                size_t physicsIterations = 0u;
                while(physicsIterations < physics::MaxPhysicsIterations && time->GetAccumulatedTime() > fixedTimeStep)
                {
                    /** @todo need to store prev transforms for interpolation */
                    physics->DoPhysicsStep(m_taskExecutor);
                    time->DecrementAccumulatedTime(fixedTimeStep);
                    ++physicsIterations;
                }
            }

            m_context.registry->CommitStagedChanges();
            frame_render();
            m_context.particleSystem->ProcessFrameEvents();
            frame_cleanup();
        }

        shutdown();
    #endif
    }

    void engine_executor::build_task_graph()
    {
        V_LOG("engine_executor::build_task_graph()");

        auto frameLogicTask = m_tasks.AddGuardedTask([this]
        {
            OPTICK_CATEGORY("Frame Logic", Optick::Category::GameLogic);
            run_frame_logic(m_dt);
        }).name("Frame Logic");

        auto writeAudioBuffersTask = m_tasks.AddGuardedTask([audioSystem = m_context.audioSystem.get()]
        {
            OPTICK_CATEGORY("AudioSystem::Update", Optick::Category::Audio);
            audioSystem->Update();
        }).name("Write Audio Buffers");

        auto particleSystemTask = m_tasks.AddGuardedTask(
            [particleEmitterSystem = m_context.particleSystem.get(), &dt = m_dt]
        {
            OPTICK_CATEGORY("Update Particles", Optick::Category::VFX);
            particleEmitterSystem->UpdateParticles(dt);
        }).name("Update Particles");

        auto renderTask = m_tasks.AddGuardedTask([this]
        {
            OPTICK_CATEGORY("FrameRender", Optick::Category::Rendering);
            m_context.registry->CommitStagedChanges();
            frame_render();
        }).name("Commit Changes and Render");

        auto endFrameTask = m_tasks.AddGuardedTask([this]
        {
            m_context.particleSystem->ProcessFrameEvents();
            frame_cleanup();
        }).name("Frame Cleanup");

        auto& engineTF = m_tasks.GetTaskFlow();
        auto physicsFinished = engineTF.emplace([]{}).name("Physics Finished");

        if(m_context.physicsSystem)
        {
            auto& physicsTF = m_context.physicsSystem->GetTasks().GetTaskFlow();
            auto physicsModule = engineTF.composed_of(physicsTF).name("Physics Module");

            auto physicsStepCondition = engineTF.emplace(
                [&curIt = m_currentPhysicsIterations,
                maxIt = physics::MaxPhysicsIterations,
                time = m_context.time.get(),
                fixedStep = config::GetPhysicsSettings().fixedUpdateInterval]
            {
                return (curIt < maxIt && time->GetAccumulatedTime() > fixedStep) ? 1 : 0;
            }).name("Condition: Do Physics Step");

            auto updatePhysicsCondition = engineTF.emplace(
                [&i = m_currentPhysicsIterations,
                time = m_context.time.get(),
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

    void engine_executor::clear()
    {
        V_LOG("engine_executor::clear()");
        nc::clear(&m_context);
    }

    void engine_executor::do_scene_swap()
    {
        V_LOG("engine_executor::do_scene_swap()");
        m_context.sceneSystem->UnloadActiveScene();
        clear();
        m_context.sceneSystem->DoSceneChange(m_engine);
    }

    void engine_executor::run_frame_logic(float dt)
    {
        auto* registry = m_context.registry.get();

        for(auto& frameLogic : view<FrameLogic>{registry})
            frameLogic.Run(registry, dt);
    }

    void engine_executor::frame_render()
    {
        if(!m_context.application) return;

        /** Update the view matrix for the camera */
        auto* mainCamera = m_context.mainCamera->Get();
        mainCamera->UpdateViewMatrix();

        /** Setup the frame */
        if (!m_context.application->frame_begin()) return;

        auto* registry = m_context.registry.get();

        #ifdef NC_EDITOR_ENABLED
        if(auto* ui = m_context.application->get_ui()) ui->Draw(&m_frameDeltaTimeFactor, registry);
        #else
        if(auto* ui = m_context.application->get_ui()) ui->Draw();
        #endif

        /** Get the frame data */
        auto state = graphics::PerFrameRenderState{registry, mainCamera, m_context.pointLightSystem->CheckDirtyAndReset(), m_context.environment.get()};
        graphics::MapPerFrameRenderState(state);

        /** Draw the frame */
        m_context.application->draw(state);

        #ifdef NC_EDITOR_ENABLED
        for(auto& collider : view<Collider>{registry}) collider.SetEditorSelection(false);
        #endif

        /** End the frame */
        m_context.application->frame_end();
    }

    void engine_executor::frame_cleanup()
    {
        if(m_context.sceneSystem->IsSceneChangeScheduled())
        {
            do_scene_swap();
        }

        input::Flush();
    }
}