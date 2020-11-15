#include "Engine.h"
#include "EngineImpl.h"
#include "DebugUtils.h"

namespace nc::engine
{
    std::function<const config::Config&()> Engine::GetConfig_ = nullptr;
    std::function<void(std::string)> Engine::SetUserName_ = nullptr;

Engine::Engine(HINSTANCE hInstance)
    : m_impl{ std::make_unique<EngineImpl>(hInstance, std::bind(this->Shutdown, this, std::placeholders::_1)) }
{
}

Engine::~Engine()
{
}

void Engine::Start(std::unique_ptr<scene::Scene> initialScene)
{
    V_LOG("Starting engine");
    m_impl->MainLoop(std::move(initialScene));
}

void Engine::Shutdown(bool forceImmediate)
{
    V_LOG("Shutting down engine - forceImmediate=" + std::to_string(forceImmediate));

    if (forceImmediate)
    {
        m_impl->Shutdown();
    }
    else
    {
        m_impl->isRunning = false;
    }
}

const config::Config& Engine::GetConfig()
{
    return Engine::GetConfig_();
}

void Engine::SetUserName(std::string name)
{
    Engine::SetUserName_(std::move(name));
}
}// end namespace nc::engine