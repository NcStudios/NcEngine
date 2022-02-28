#include "Engine.h"
#include "config/Config.h"
#include "config/ConfigInternal.h"
#include "debug/Utils.h"

namespace nc
{
    auto InitializeNcEngine(std::string_view configPath, engine_init_flags flags) -> std::unique_ptr<NcEngine>
    {
        config::LoadInternal(configPath);
        debug::internal::OpenLog(config::GetProjectSettings().logFilePath);
        V_LOG("Constructing Engine");
        return std::make_unique<Engine>(build_engine_context(flags));
    }

    Engine::Engine(engine_context context)
        : m_executor{std::move(context), this}
    {
        V_LOG("Engine constructed");
    }

    Engine::~Engine() noexcept
    {
        debug::internal::CloseLog();
    }

    void Engine::Start(std::unique_ptr<Scene> initialScene)
    {
        m_executor.start(std::move(initialScene));
    }

    void Engine::Quit() noexcept
    {
        m_executor.stop();
    }

    void Engine::Shutdown() noexcept
    {
        m_executor.shutdown();
    }

    auto Engine::Audio()       noexcept -> AudioSystem*     { return m_executor.get_context()->audioSystem.get();     }
    auto Engine::Environment() noexcept -> nc::Environment* { return m_executor.get_context()->environment.get();     }
    auto Engine::Random()      noexcept -> nc::Random*      { return m_executor.get_context()->random.get();          }
    auto Engine::Registry()    noexcept -> nc::Registry*    { return m_executor.get_context()->registry.get();        }
    auto Engine::MainCamera()  noexcept -> nc::MainCamera*  { return m_executor.get_context()->mainCamera.get();      }
    auto Engine::Physics()     noexcept -> PhysicsSystem*   { return m_executor.get_context()->physicsSystem.get();   }
    auto Engine::SceneSystem() noexcept -> nc::SceneSystem* { return m_executor.get_context()->sceneSystem.get();     }
    auto Engine::UI()          noexcept -> UISystem*        { return m_executor.get_context()->application->get_ui(); }
} // end namespace nc