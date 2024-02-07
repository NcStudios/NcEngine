#pragma once

#include "NcEngine.h"
#include "ecs/ComponentRegistry.h"
#include "math/Random.h"
#include "task/Executor.h"
#include "window/WindowImpl.h"

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
            void RebuildTaskGraph() override;

        private:
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
