#pragma once

#include "NcEngine.h"
#include "assets/AssetManagers.h"
#include "ecs/Registry.h"
#include "math/Random.h"
#include "scene/SceneManager.h"
#include "task/Executor.h"
#include "time/Time.h"
#include "window/WindowImpl.h"

namespace nc
{
    class NcEngineImpl : public NcEngine
    {
        public:
            NcEngineImpl(EngineInitFlags flags);
            ~NcEngineImpl() noexcept;
            void Start(std::unique_ptr<Scene> initialScene) override;
            void Stop() noexcept override;
            void Shutdown() noexcept override;
            void QueueSceneChange(std::unique_ptr<Scene> scene) override;
            bool IsSceneChangeQueued() const noexcept override;
            auto GetRegistry() noexcept -> Registry* override;
            auto GetModuleRegistry() noexcept -> ModuleRegistry* override;
            void RebuildTaskGraph() override;

        private:
            window::WindowImpl m_window;
            nc::Registry m_registry;
            time::Time m_time;
            nc::AssetManagers m_assets;
            ModuleRegistry m_modules;
            task::Executor m_executor;
            scene::SceneManager m_sceneManager;
            float m_dt;
            bool m_isRunning;

            void Clear();
            void Run();
    };
}