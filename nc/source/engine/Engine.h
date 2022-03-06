#pragma once

#include "NcEngine.h"
#include "engine_executor.h"

namespace nc
{
    class Engine final : public NcEngine
    {
        public:
            Engine(engine_context context);
            ~Engine() noexcept;

            void Start(std::unique_ptr<Scene> initialScene) override;
            void Quit() noexcept override;
            void Shutdown() noexcept override;
            auto Audio() noexcept -> AudioSystem* override;
            auto Environment() noexcept -> nc::Environment* override;
            auto MainCamera() noexcept -> nc::MainCamera* override;
            auto Random() noexcept -> nc::Random* override;
            auto Registry() noexcept -> nc::Registry* override;
            auto Physics() noexcept -> PhysicsSystem* override;
            auto SceneSystem() noexcept -> nc::SceneSystem* override;
            auto UI() noexcept -> UISystem* override;

        private:
            engine_executor m_executor;
    };
} // end namespace nc::engine