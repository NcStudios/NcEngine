#include "NcEngineImpl.h"
#include "audio/NcAudioImpl.h"
#include "config/ConfigInternal.h"
#include "ecs/LogicModule.h"
#include "graphics/NcGraphicsImpl.h"
#include "graphics/PerFrameRenderState.h"
#include "input/InputInternal.h"
#include "module/ModuleProvider.h"
#include "physics/NcPhysicsImpl.h"
#include "time/TimeImpl.h"
#include "utility/Log.h"

namespace
{
auto BuildModuleRegistry(nc::Registry* reg,
                         nc::window::WindowImpl* window,
                         const nc::GpuAccessorSignals& gpuAccessorSignals,
                         const nc::config::Config& config) -> nc::ModuleRegistry
{
    NC_LOG_INFO("Building module registry");
    auto moduleRegistry = nc::ModuleRegistry{};
    moduleRegistry.Register(nc::graphics::BuildGraphicsModule(config.projectSettings, config.graphicsSettings, gpuAccessorSignals, reg, window));
    moduleRegistry.Register(nc::physics::BuildPhysicsModule(config.physicsSettings, reg));
    moduleRegistry.Register(nc::audio::BuildAudioModule(config.audioSettings, reg));
    moduleRegistry.Register(nc::time::BuildTimeModule());
    moduleRegistry.Register(nc::ecs::BuildLogicModule(reg));
    moduleRegistry.Register(std::make_unique<nc::Random>());
    return moduleRegistry;
}
} // anonymous namespace

namespace nc
{
auto InitializeNcEngine(const config::Config& config) -> std::unique_ptr<NcEngine>
{
    config::SetConfig(config);
    utility::detail::InitializeLog(config.projectSettings.logFilePath);
    NC_LOG_INFO("Creating NcEngine instance");
    return std::make_unique<NcEngineImpl>(config);
}

NcEngineImpl::NcEngineImpl(const config::Config& config)
    : m_window{std::bind_front(&NcEngineImpl::Stop, this)},
      m_registry{config.memorySettings.maxTransforms},
      m_assets{config.assetSettings, config.memorySettings},
      m_modules{BuildModuleRegistry(&m_registry, &m_window, m_assets.CreateGpuAccessorSignals(), config)},
      m_executor{},
      m_sceneManager{std::bind_front(&NcEngineImpl::Clear, this)},
      m_isRunning{false}
{
}

NcEngineImpl::~NcEngineImpl() noexcept
{
    Shutdown();
    utility::detail::CloseLog();
}

void NcEngineImpl::Start(std::unique_ptr<Scene> initialScene)
{
    NC_LOG_INFO("Starting engine");
    RebuildTaskGraph();
    m_sceneManager.QueueSceneChange(std::move(initialScene));
    m_sceneManager.DoSceneChange(&m_registry, ModuleProvider{&m_modules});
    m_isRunning = true;
    Run();
}

void NcEngineImpl::Stop() noexcept
{
    NC_LOG_INFO("Stopping engine");
    m_isRunning = false;
}

void NcEngineImpl::Shutdown() noexcept
{
    NC_LOG_INFO("Shutting down engine");
    try
    {
        Clear();
    }
    catch (const std::exception& e)
    {
        NC_LOG_EXCEPTION(e);
    }
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
    NC_LOG_INFO("Building engine task graph");
    m_executor.BuildTaskGraph(&m_registry, m_modules.GetAllModules());
}

void NcEngineImpl::Clear()
{
    NC_LOG_TRACE("Clearing engine state");
    m_modules.Clear();
    m_registry.Clear();
}

void NcEngineImpl::Run()
{
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
} // namespace nc
