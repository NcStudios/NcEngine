#include "Runtime.h"
#include "audio/AudioModuleImpl.h"
#include "config/ConfigInternal.h"
#include "graphics/GraphicsModuleImpl.h"
#include "graphics/PerFrameRenderState.h"
#include "input/InputInternal.h"
#include "physics/PhysicsModuleImpl.h"
#include "scene/SceneModuleImpl.h"

namespace
{
    auto BuildModules(nc::Registry* reg,
                      nc::window::WindowImpl* window,
                      nc::time::Time* time,
                      const nc::GpuAccessorSignals& gpuAccessorSignals,
                      std::function<void()> clearCallback,
                      float* dt, nc::EngineInitFlags flags) -> nc::Modules
    {
        V_LOG("BuildModules()");
        bool enableGraphicsModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoGraphics);
        bool enablePhysicsModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoPhysics);
        bool enableAudioModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoAudio);
        return nc::Modules
        {
            .graphicsModule = nc::graphics::BuildGraphicsModule(enableGraphicsModule, reg, gpuAccessorSignals, window, dt),
            .physicsModule = nc::physics::BuildPhysicsModule(enablePhysicsModule, reg, time),
            .audioModule = nc::audio::BuildAudioModule(enableAudioModule, reg),
            .sceneModule = std::make_unique<nc::scene::SceneModuleImpl>(std::move(clearCallback)),
            .logicModule = std::make_unique<nc::LogicModule>(reg, dt)
        };
    }
}

namespace nc
{
    auto InitializeNcEngine(std::string_view configPath, EngineInitFlags flags) -> std::unique_ptr<NcEngine>
    {
        config::LoadInternal(configPath);
        debug::internal::OpenLog(config::GetProjectSettings().logFilePath);
        V_LOG("Initializing Runtime");
        return std::make_unique<Runtime>(flags);
    }

    Runtime::Runtime(EngineInitFlags flags)
        : m_window{},
          m_registry{nc::config::GetMemorySettings().maxTransforms},
          m_time{},
          m_random{},
          m_assets{nc::config::GetProjectSettings(), nc::config::GetMemorySettings()},
          m_modules{ BuildModules(&m_registry, &m_window, &m_time, m_assets.CreateGpuAccessorSignals(), std::bind_front(&Runtime::Clear, this), &m_dt, flags) },
          m_executor{},
          m_dt{ 0.0f },
          m_dtFactor{ 1.0f },
          m_isRunning{ false },
          m_currentPhysicsIterations{ 0u }
    {
        m_window.BindEngineDisableRunningCallback(std::bind_front(&Runtime::Stop, this));
        BuildTaskGraph();
        V_LOG("Runtime Initialized");
    }

    Runtime::~Runtime() noexcept
    {
        Shutdown();
    }

    void Runtime::Start(std::unique_ptr<nc::Scene> initialScene)
    {
        V_LOG("Runtime::Start()");
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
        catch (const std::exception& e)
        {
            debug::LogException(e);
        }

        debug::internal::CloseLog();
    }

    auto Runtime::Audio()    noexcept -> AudioModule* { return m_modules.audioModule.get(); }
    auto Runtime::Graphics() noexcept -> GraphicsModule* { return m_modules.graphicsModule.get(); }
    auto Runtime::Physics()  noexcept -> PhysicsModule* { return m_modules.physicsModule.get(); }
    auto Runtime::Random()   noexcept -> nc::Random* { return &m_random; }
    auto Runtime::Registry() noexcept -> nc::Registry* { return &m_registry; }
    auto Runtime::Scene()    noexcept -> SceneModule* { return m_modules.sceneModule.get(); }

    void Runtime::BuildTaskGraph()
    {
        V_LOG("Runtime::BuildTaskGraph()");

        auto syncJob = [reg = &m_registry]
        {
            OPTICK_CATEGORY("Sync State", Optick::Category::None);
            reg->CommitStagedChanges();
        };
        m_executor.Add(m_modules.audioModule->BuildWorkload());
        m_executor.Add(m_modules.logicModule->BuildWorkload());
        m_executor.Add(m_modules.physicsModule->BuildWorkload());
        m_executor.Add(m_modules.graphicsModule->BuildWorkload());
        m_executor.Add(Job{ syncJob, "Sync", HookPoint::PreRenderSync });
        m_executor.Build();
    }

    void Runtime::Clear()
    {
        m_modules.graphicsModule->Clear();
        m_registry.Clear();
        m_modules.physicsModule->Clear();
        m_modules.audioModule->Clear();
        m_time.ResetFrameDeltaTime();
        m_time.ResetAccumulatedTime();
    }

    void Runtime::Run()
    {
        while(m_isRunning)
        {
            OPTICK_FRAME("Main Thread");
            m_dt = m_dtFactor * m_time.UpdateTime();
            m_currentPhysicsIterations = 0u;
            input::Flush();
            m_window.ProcessSystemMessages();
            auto result = m_executor.Run();
            result.wait();
            m_modules.sceneModule->DoSceneSwap(this);
        }

        Shutdown();
    }
}