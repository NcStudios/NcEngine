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
                         nc::EngineInitFlags flags) -> nc::ModuleRegistry
{
    NC_LOG_INFO("Building module registry");
    const bool enableGraphics = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoGraphics);
    const bool enablePhysics = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoPhysics);
    const bool enableAudio = nc::EngineInitFlags::None == (flags & nc::EngineInitFlags::NoAudio);
    auto moduleRegistry = nc::ModuleRegistry{};
    moduleRegistry.Register(nc::graphics::BuildGraphicsModule(enableGraphics, reg, gpuAccessorSignals, window));
    moduleRegistry.Register(nc::physics::BuildPhysicsModule(enablePhysics, reg));
    moduleRegistry.Register(nc::audio::BuildAudioModule(enableAudio, reg));
    moduleRegistry.Register(nc::time::BuildTimeModule());
    moduleRegistry.Register(std::make_unique<nc::ecs::LogicModule>(reg));
    moduleRegistry.Register(std::make_unique<nc::Random>());
    return moduleRegistry;
}
} // anonymous namespace

namespace nc
{
auto InitializeNcEngine(const config::Config& config, EngineInitFlags flags) -> std::unique_ptr<NcEngine>
{
    config::SetConfig(config);
    utility::detail::InitializeLog(config::GetProjectSettings().logFilePath);
    NC_LOG_INFO("Creating NcEngine instance");
    NC_LOG_INFO_FMT(R"(EngineInitFlags\n\tDisable Audio: {}\n\tDisable Graphics: {}\n\tDisable Physics: {})",
                    static_cast<int>(flags & nc::EngineInitFlags::NoAudio),
                    static_cast<int>(flags & nc::EngineInitFlags::NoGraphics),
                    static_cast<int>(flags & nc::EngineInitFlags::NoPhysics));
    return std::make_unique<NcEngineImpl>(flags);
}

NcEngineImpl::NcEngineImpl(EngineInitFlags flags)
    : m_window{std::bind_front(&NcEngineImpl::Stop, this)},
      m_registry{nc::config::GetMemorySettings().maxTransforms},
      m_assets{nc::config::GetAssetSettings(), nc::config::GetMemorySettings()},
      m_modules{BuildModuleRegistry(&m_registry, &m_window, m_assets.CreateGpuAccessorSignals(), flags)},
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
