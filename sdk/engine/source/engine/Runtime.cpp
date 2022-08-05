#include "Runtime.h"
#include "audio/AudioModuleImpl.h"
#include "config/ConfigInternal.h"
#include "ecs/LogicModule.h"
#include "graphics/GraphicsModuleImpl.h"
#include "graphics/PerFrameRenderState.h"
#include "input/InputInternal.h"
#include "module/ModuleProvider.h"
#include "physics/PhysicsModuleImpl.h"
#include "scene/SceneModuleImpl.h"

namespace
{
    auto BuildModuleRegistry(nc::Registry* reg,
                             nc::window::WindowImpl* window,
                             nc::time::Time* time,
                             const nc::GpuAccessorSignals& gpuAccessorSignals,
                             std::function<void()> clearCallback,
                             float* dt, nc::EngineInitFlags flags) -> nc::ModuleRegistry
    {
        V_LOG("BuildModules()");
        const bool enableGraphicsModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoGraphics);
        const bool enablePhysicsModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoPhysics);
        const bool enableAudioModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoAudio);

        auto graphics = nc::graphics::BuildGraphicsModule(enableGraphicsModule, reg, gpuAccessorSignals, window, dt);
        auto physics = nc::physics::BuildPhysicsModule(enablePhysicsModule, reg, time);
        auto audio = nc::audio::BuildAudioModule(enableAudioModule, reg);
        auto scene = nc::scene::BuildSceneModule(std::move(clearCallback));
        auto logic = std::make_unique<nc::LogicModule>(reg, dt);
        auto random = std::make_unique<nc::Random>();

        auto moduleRegistry = nc::ModuleRegistry{};
        moduleRegistry.Register(std::move(graphics));
        moduleRegistry.Register(std::move(physics));
        moduleRegistry.Register(std::move(audio));
        moduleRegistry.Register(std::move(scene));
        moduleRegistry.Register(std::move(logic));
        moduleRegistry.Register(std::move(random));
        return moduleRegistry;
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
        : m_window{std::bind_front(&Runtime::Stop, this)},
          m_registry{nc::config::GetMemorySettings().maxTransforms},
          m_time{},
          m_assets{nc::config::GetProjectSettings(), nc::config::GetMemorySettings()},
          m_modules{BuildModuleRegistry(&m_registry, &m_window, &m_time,
                                        m_assets.CreateGpuAccessorSignals(),
                                        std::bind_front(&Runtime::Clear, this),
                                        &m_dt, flags)},
          m_executor{},
          m_dt{0.0f},
          m_isRunning{false}
    {
        V_LOG("Runtime Initialized");
    }

    Runtime::~Runtime() noexcept
    {
        Shutdown();
    }

    void Runtime::Start(std::unique_ptr<Scene> initialScene)
    {
        V_LOG("Runtime::Start()");
        RebuildTaskGraph();
        auto* sceneModule = m_modules.Get<SceneModule>();
        sceneModule->ChangeScene(std::move(initialScene));
        sceneModule->DoSceneSwap(&m_registry, ModuleProvider{&m_modules});
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

    auto Runtime::GetRegistry() noexcept -> Registry*
    {
        return &m_registry;
    }

    auto Runtime::GetModuleRegistry() noexcept -> ModuleRegistry*
    {
        return &m_modules;
    }

    void Runtime::RebuildTaskGraph()
    {
        V_LOG("Runtime::RebuildTaskGraph()");
        m_executor.BuildTaskGraph(&m_registry, m_modules.GetAllModules());
    }

    void Runtime::Clear()
    {
        V_LOG("Runtime::Clear()");
        m_modules.Clear();
        m_registry.Clear();
        m_time.ResetFrameDeltaTime();
        m_time.ResetAccumulatedTime();
    }

    void Runtime::Run()
    {
        V_LOG("Runtime::Run()");
        while(m_isRunning)
        {
            OPTICK_FRAME("Main Thread");
            m_dt = m_time.UpdateTime();
            input::Flush();
            m_window.ProcessSystemMessages();
            auto result = m_executor.Run();
            result.wait();
            auto* sceneModule = m_modules.Get<SceneModule>();
            if (sceneModule->IsSceneChangeScheduled())
            {
                sceneModule->DoSceneSwap(&m_registry, ModuleProvider{&m_modules});
            }
        }

        Shutdown();
    }
}