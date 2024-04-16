#pragma once

#include "task/Executor.h"
#include "time/StepTimer.h"
#include "window/WindowImpl.h"

#include "ncengine/NcEngine.h"
#include "ncengine/ecs/ComponentRegistry.h"
#include "ncengine/math/Random.h"

namespace nc
{
    class NcEngineImpl : public NcEngine
    {
        public:
            NcEngineImpl(const config::Config& config);
            ~NcEngineImpl() noexcept;
            void Start(std::unique_ptr<Scene> initialScene) override;
            void Stop() noexcept override;
            void Shutdown() noexcept override;
            auto GetComponentRegistry() noexcept -> ecs::ComponentRegistry& override;
            auto GetModuleRegistry() noexcept -> ModuleRegistry* override;
            auto GetSystemEvents() noexcept -> SystemEvents& override;
            void RebuildTaskGraph() override;

        private:
            time::StepTimer m_timer;
            SystemEvents m_events;
            window::WindowImpl m_window;
            std::unique_ptr<ecs::ComponentRegistry> m_registry;
            Registry m_legacyRegistry; // delete once all usage is cutover
            std::unique_ptr<ModuleRegistry> m_modules;
            task::Executor m_executor;
            bool m_isRunning;

            void ClearScene();
            void Run();
    };
}
