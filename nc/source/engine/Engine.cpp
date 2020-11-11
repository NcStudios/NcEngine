#include "Engine.h"
#include "EngineImpl.h"
#include "NcDebug.h"

namespace nc::engine
{
Engine::Engine(HINSTANCE hInstance)
    : m_impl{ std::make_unique<EngineImpl>(hInstance, this) }
{
    V_LOG("Creating engine");
}

Engine::~Engine()
{
}

void Engine::Start()
{
    V_LOG("Starting engine");
    m_impl->MainLoop();
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
}// end namespace nc::engine