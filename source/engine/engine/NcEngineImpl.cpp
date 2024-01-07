#include "NcEngineImpl.h"
#include "ModuleFactory.h"
#include "RegistryFactories.h"
#include "config/ConfigInternal.h"
#include "config/Version.h"
#include "input/InputInternal.h"
#include "utility/Log.h"

#include "optick.h"

#include <sstream>

namespace
{
void LogConfig(const nc::config::Config& config)
{
    auto stream = std::stringstream{};
    nc::config::Write(stream, config, false);
    auto configStr = std::string{"\t"};
    configStr.reserve(static_cast<size_t>(stream.tellp()) + 50);

    // Indent contents for readability in the log
    for (auto c : stream.str())
    {
        configStr.push_back(c);
        if (c == '\n')
        {
            configStr.push_back('\t');
        }
    }

    NC_LOG_INFO("Initializing NcEngine with Config:\n{}", configStr);
}
} // anonymous namespace

namespace nc
{
auto InitializeNcEngine(const config::Config& config) -> std::unique_ptr<NcEngine>
{
    config::SetConfig(config);
    utility::detail::InitializeLog(config.projectSettings);
    ::LogConfig(config);
    NC_LOG_INFO("Creating NcEngine instance v{}", NC_PROJECT_VERSION);
    return std::make_unique<NcEngineImpl>(config);
}

NcEngineImpl::NcEngineImpl(const config::Config& config)
    : m_window{config.projectSettings, config.graphicsSettings, std::bind_front(&NcEngineImpl::Stop, this)},
      m_registry{BuildRegistry(config.memorySettings.maxTransforms)},
      m_modules{BuildModuleRegistry(m_registry.get(), &m_window, config)},
      m_executor{task::BuildContext(m_modules.GetAllModules())},
      m_sceneManager{},
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
    m_isRunning = true;
    QueueSceneChange(std::move(initialScene));
    LoadScene();
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
    return m_registry.get();
}

auto NcEngineImpl::GetModuleRegistry() noexcept -> ModuleRegistry*
{
    return &m_modules;
}

void NcEngineImpl::RebuildTaskGraph()
{
    NC_LOG_INFO("Building engine task graph");
    m_executor.SetContext(task::BuildContext(m_modules.GetAllModules()));
}

void NcEngineImpl::LoadScene()
{
    for (auto& module : m_modules.GetAllModules())
    {
        module->OnBeforeSceneLoad();
    }

    m_sceneManager.LoadQueuedScene(m_registry.get(), ModuleProvider{&m_modules});
}

void NcEngineImpl::Clear()
{
    NC_LOG_TRACE("Clearing engine state");
    m_sceneManager.UnloadActiveScene();
    for (auto& module : m_modules.GetAllModules())
    {
        module->Clear();
    }

    m_registry->Clear();
}

void NcEngineImpl::Run()
{
    while(m_isRunning)
    {
        OPTICK_FRAME("Main Thread");
        input::Flush();
        m_window.ProcessSystemMessages();
        m_executor.Run();
        if (IsSceneChangeQueued())
        {
            Clear();
            LoadScene();
        }
    }

    Shutdown();
}
} // namespace nc
