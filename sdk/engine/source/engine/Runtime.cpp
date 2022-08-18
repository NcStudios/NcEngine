#include "Runtime.h"
#include "audio/AudioModuleImpl.h"
#include "config/ConfigInternal.h"
#include "ecs/LogicModule.h"
#include "graphics/GraphicsModuleImpl.h"
#include "graphics/PerFrameRenderState.h"
#include "input/InputInternal.h"
#include "module/ModuleProvider.h"
#include "physics/PhysicsModuleImpl.h"
#include "time/TimeImpl.h"

namespace
{
    auto BuildModuleRegistry(nc::Registry* reg,
                             nc::window::WindowImpl* window,
                             const nc::GpuAccessorSignals& gpuAccessorSignals,
                             nc::EngineInitFlags flags) -> nc::ModuleRegistry
    {
        V_LOG("BuildModules()");
        const bool enableGraphicsModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoGraphics);
        const bool enablePhysicsModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoPhysics);
        const bool enableAudioModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoAudio);

        auto graphics = nc::graphics::BuildGraphicsModule(enableGraphicsModule, reg, gpuAccessorSignals, window);
        auto physics = nc::physics::BuildPhysicsModule(enablePhysicsModule, reg);
        auto audio = nc::audio::BuildAudioModule(enableAudioModule, reg);
        auto time = nc::time::BuildTimeModule();
        auto logic = std::make_unique<nc::ecs::LogicModule>(reg);
        auto random = std::make_unique<nc::Random>();

        auto moduleRegistry = nc::ModuleRegistry{};
        moduleRegistry.Register(std::move(graphics));
        moduleRegistry.Register(std::move(physics));
        moduleRegistry.Register(std::move(audio));
        moduleRegistry.Register(std::move(time));
        moduleRegistry.Register(std::move(logic));
        moduleRegistry.Register(std::move(random));
        return moduleRegistry;
    }
}

namespace nc
{
    auto InitializeNcEngine(const config::Config& config, EngineInitFlags flags) -> std::unique_ptr<NcEngine>
    {
        config::SetConfig(config);
        debug::internal::OpenLog(config::GetProjectSettings().logFilePath);
        V_LOG("Initializing Runtime");
        return std::make_unique<Runtime>(flags);
    }

    Runtime::Runtime(EngineInitFlags flags)
        : m_window{std::bind_front(&Runtime::Stop, this)},
          m_registry{nc::config::GetMemorySettings().maxTransforms},
          m_assets{nc::config::GetAssetSettings(), nc::config::GetMemorySettings()},
          m_modules{BuildModuleRegistry(&m_registry, &m_window, m_assets.CreateGpuAccessorSignals(), flags)},
          m_executor{},
          m_sceneManager{std::bind_front(&Runtime::Clear, this)},
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
        m_sceneManager.QueueSceneChange(std::move(initialScene));
        m_sceneManager.DoSceneChange(&m_registry, ModuleProvider{&m_modules});
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

    bool Runtime::IsSceneChangeQueued() const noexcept
    {
        return m_sceneManager.IsSceneChangeQueued();
    }

    void Runtime::QueueSceneChange(std::unique_ptr<Scene> scene)
    {
        m_sceneManager.QueueSceneChange(std::move(scene));
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
    }

    void Runtime::Run()
    {
        V_LOG("Runtime::Run()");
        while(m_isRunning)
        {
            OPTICK_FRAME("Main Thread");
            input::Flush();
            m_window.ProcessSystemMessages();
            auto result = m_executor.Run();
            result.wait();
            if (m_sceneManager.IsSceneChangeQueued())
            {
                m_sceneManager.DoSceneChange(&m_registry, ModuleProvider{&m_modules});
            }
        }

        Shutdown();
    }
}