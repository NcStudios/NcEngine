#include "NcEngineImpl.h"
#include "audio/NcAudioImpl.h"
#include "config/ConfigInternal.h"
#include "ecs/LogicModule.h"
#include "graphics/GraphicsModuleImpl.h"
#include "graphics/PerFrameRenderState.h"
#include "input/InputInternal.h"
#include "module/ModuleProvider.h"
#include "physics/NcPhysicsImpl.h"

namespace
{
    auto BuildModuleRegistry(nc::Registry* reg,
                             nc::window::WindowImpl* window,
                             nc::time::Time* time,
                             const nc::GpuAccessorSignals& gpuAccessorSignals,
                             float* dt, nc::EngineInitFlags flags) -> nc::ModuleRegistry
    {
        V_LOG("BuildModuleRegistry()");
        const bool enableGraphicsModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoGraphics);
        const bool enablePhysicsModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoPhysics);
        const bool enableAudioModule = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoAudio);

        auto graphics = nc::graphics::BuildGraphicsModule(enableGraphicsModule, reg, gpuAccessorSignals, window, dt);
        auto physics = nc::physics::BuildPhysicsModule(enablePhysicsModule, reg, time);
        auto audio = nc::audio::BuildAudioModule(enableAudioModule, reg);
        auto logic = std::make_unique<nc::LogicModule>(reg, dt);
        auto random = std::make_unique<nc::Random>();

        auto moduleRegistry = nc::ModuleRegistry{};
        moduleRegistry.Register(std::move(graphics));
        moduleRegistry.Register(std::move(physics));
        moduleRegistry.Register(std::move(audio));
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
        V_LOG("InitializingNcEngine()");
        return std::make_unique<NcEngineImpl>(flags);
    }

    NcEngineImpl::NcEngineImpl(EngineInitFlags flags)
        : m_window{std::bind_front(&NcEngineImpl::Stop, this)},
          m_registry{nc::config::GetMemorySettings().maxTransforms},
          m_time{},
          m_assets{nc::config::GetProjectSettings(), nc::config::GetMemorySettings()},
          m_modules{BuildModuleRegistry(&m_registry, &m_window, &m_time,
                                        m_assets.CreateGpuAccessorSignals(),
                                        &m_dt, flags)},
          m_executor{},
          m_sceneManager{std::bind_front(&NcEngineImpl::Clear, this)},
          m_dt{0.0f},
          m_isRunning{false}
    {
        V_LOG("NcEngine Initialized");
    }

    NcEngineImpl::~NcEngineImpl() noexcept
    {
        Shutdown();
    }

    void NcEngineImpl::Start(std::unique_ptr<Scene> initialScene)
    {
        V_LOG("NcEngine::Start()");
        RebuildTaskGraph();
        m_sceneManager.QueueSceneChange(std::move(initialScene));
        m_sceneManager.DoSceneChange(&m_registry, ModuleProvider{&m_modules});
        m_isRunning = true;
        Run();
    }

    void NcEngineImpl::Stop() noexcept
    {
        V_LOG("NcEngine::Stop()");
        m_isRunning = false;
    }

    void NcEngineImpl::Shutdown() noexcept
    {
        V_LOG("NcEngine::Shutdown()");
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

    bool NcEngineImpl::IsSceneChangeQueued() const noexcept
    {
        return m_sceneManager.IsSceneChangeQueued();
    }

    void NcEngineImpl::QueueSceneChange(std::unique_ptr<Scene> scene)
    {
        m_sceneManager.QueueSceneChange(std::move(scene));
    }

    auto NcEngineImpl::GetRegistry() noexcept -> Registry*
    {
        return &m_registry;
    }

    auto NcEngineImpl::GetModuleRegistry() noexcept -> ModuleRegistry*
    {
        return &m_modules;
    }

    void NcEngineImpl::RebuildTaskGraph()
    {
        V_LOG("NcEngine::RebuildTaskGraph()");
        m_executor.BuildTaskGraph(&m_registry, m_modules.GetAllModules());
    }

    void NcEngineImpl::Clear()
    {
        V_LOG("NcEngine::Clear()");
        m_modules.Clear();
        m_registry.Clear();
        m_time.ResetFrameDeltaTime();
        m_time.ResetAccumulatedTime();
    }

    void NcEngineImpl::Run()
    {
        V_LOG("NcEngine::Run()");
        while(m_isRunning)
        {
            OPTICK_FRAME("Main Thread");
            m_dt = m_time.UpdateTime();
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