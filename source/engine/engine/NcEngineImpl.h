#pragma once

#include "NcEngine.h"
#include "ecs/Registry.h"
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
            auto GetRegistry() noexcept -> Registry* override;
            auto GetModuleRegistry() noexcept -> ModuleRegistry* override;
            void RebuildTaskGraph() override;

        private:
            window::WindowImpl m_window;
            std::unique_ptr<Registry> m_registry;
            ModuleRegistry m_modules;
            task::Executor m_executor;
            bool m_isRunning;

            void Clear();
            void Run();
    };
}
