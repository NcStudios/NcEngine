#include "Engine.h"
#include "EngineImpl.h"
#include "DebugUtils.h"

namespace nc::engine
{
    std::function<const config::Config&()> Engine::GetConfig_ = nullptr;

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
        IF_THROW(!m_impl, "Engine::m_impl is not bound");
        m_impl->MainLoop(std::move(initialScene));
    }

    void Engine::Shutdown(bool forceImmediate)
    {
        V_LOG("Shutting down engine - forceImmediate=" + std::to_string(forceImmediate));
        IF_THROW(!m_impl, "Engine::m_impl is not bound");
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
        IF_THROW(!Engine::GetConfig_, "Engine::GetConfig_ is not bound");
        return Engine::GetConfig_();
    }
}// end namespace nc::engine