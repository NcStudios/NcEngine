#pragma once

#include "Context.h"
#include "Modules.h"
#include "NcEngine.h"
#include "task/Executor.h"
#include "window/WindowImpl.h"

namespace nc
{
    class Runtime : public NcEngine
    {
        public:
            Runtime(EngineInitFlags flags);
            ~Runtime() noexcept;
            void Start(std::unique_ptr<nc::Scene> initialScene) override;
            void Stop() noexcept override;
            void Shutdown() noexcept override;
            auto Audio() noexcept -> AudioModule* override;
            auto Graphics() noexcept -> GraphicsModule* override;
            auto Physics() noexcept -> PhysicsModule* override;
            auto Random() noexcept -> nc::Random* override;
            auto Registry() noexcept -> nc::Registry* override;
            auto Scene() noexcept -> SceneModule* override;
            // auto Assets() noexcept -> nc::Assets* override;

        private:
            window::WindowImpl m_window;
            Context m_context;
            Modules m_modules;
            Executor m_executor;
            float m_dt;
            float m_dtFactor;
            bool m_isRunning;
            unsigned m_currentPhysicsIterations; /** @todo should go in physics_module_impl */

            void BuildTaskGraph(); /** @todo could maybe have graph builder class/func */
            void Clear();
            void Run();
    };
}