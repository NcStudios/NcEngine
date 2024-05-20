#include "NcEngineImpl.h"
#include "ModuleFactory.h"
#include "RegistryFactory.h"
#include "config/ConfigInternal.h"
#include "config/Version.h"
#include "input/InputInternal.h"
#include "scene/NcScene.h"
#include "time/TimeImpl.h"
#include "utility/Log.h"

#include "optick.h"

#include <sstream>
#include <thread>

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

auto BuildTimer(const nc::config::EngineSettings& settings) -> nc::time::StepTimer
{
    if (settings.timeStep == 0.0f)
    {
        NC_LOG_INFO("Building variable step timer");
        return nc::time::StepTimer{settings.maxTimeStep};
    }

    NC_LOG_INFO("Building fixed step timer");
    return nc::time::StepTimer{settings.timeStep, settings.maxTimeStep};
}

auto BuildExecutor(const nc::config::EngineSettings& settings, const nc::ModuleRegistry& modules) -> nc::task::Executor
{
    const auto threadCount = settings.threadCount > 0 ? settings.threadCount : std::thread::hardware_concurrency();
    NC_LOG_INFO("Building Executor with {} threads", threadCount);
    return nc::task::Executor{threadCount, nc::task::BuildContext(modules.GetAllModules())};
}
} // anonymous namespace

namespace nc
{
auto InitializeNcEngine(const config::Config& config) -> std::unique_ptr<NcEngine>
{
    utility::detail::InitializeLog(config.projectSettings);
    NC_LOG_INFO("Creating NcEngine instance v{}", NC_PROJECT_VERSION);
    ::LogConfig(config);
    if (!config::Validate(config))
    {
        NC_LOG_ERROR("NcEngine initialization failed: invalid config");
        utility::detail::CloseLog();
        throw NcError("NcEngine initialization failed: invalid config");
    }

    config::SetConfig(config);
    return std::make_unique<NcEngineImpl>(config);
}

NcEngineImpl::NcEngineImpl(const config::Config& config)
    : m_timer{::BuildTimer(config.engineSettings)},
      m_window{config.projectSettings, config.graphicsSettings, std::bind_front(&NcEngineImpl::Stop, this)},
      m_registry{BuildRegistry(config.memorySettings.maxTransforms)},
      m_legacyRegistry{*m_registry},
      m_modules{BuildModuleRegistry(&m_legacyRegistry, m_events, &m_window, config)},
      m_executor{::BuildExecutor(config.engineSettings, *m_modules)},
      m_isRunning{false}
{
    SetActiveRegistry(&m_legacyRegistry);
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
    auto ncScene = m_modules->Get<NcScene>();
    ncScene->Queue(std::move(initialScene));
    ncScene->LoadQueuedScene(ecs::Ecs{*m_registry}, *m_modules);
    m_timer.Reset();
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
        ClearScene();
        m_registry->Clear();
    }
    catch (const std::exception& e)
    {
        NC_LOG_EXCEPTION(e);
    }
}

auto NcEngineImpl::GetComponentRegistry() noexcept -> ecs::ComponentRegistry&
{
    return *m_registry;
}

auto NcEngineImpl::GetModuleRegistry() noexcept -> ModuleRegistry*
{
    return m_modules.get();
}

auto NcEngineImpl::GetSystemEvents() noexcept -> SystemEvents&
{
    return m_events;
}

void NcEngineImpl::RebuildTaskGraph()
{
    NC_LOG_INFO("Building engine task graph");
    m_executor.SetContext(task::BuildContext(m_modules->GetAllModules()));
}

void NcEngineImpl::ClearScene()
{
    NC_LOG_TRACE("Clearing engine state");
    m_modules->Get<NcScene>()->UnloadActiveScene();
    for (auto& module : m_modules->GetAllModules())
    {
        module->Clear();
    }

    m_registry->ClearSceneData();
}

void NcEngineImpl::Run()
{
    auto* ncScene = m_modules->Get<NcScene>();
    auto update = [this](float dt)
    {
        time::SetDeltaTime(dt);
        input::Flush();
        m_window.ProcessSystemMessages();
        m_executor.RunUpdateTasks();
    };

    while(m_isRunning)
    {
        OPTICK_FRAME("Main Thread");
        if (m_timer.Tick(update))
        {
            m_executor.RunRenderTasks();
            if (ncScene->IsTransitionScheduled())
            {
                ClearScene();
                ncScene->LoadQueuedScene(ecs::Ecs{*m_registry}, *m_modules);
            }
        }
    }

    Shutdown();
}
} // namespace nc
