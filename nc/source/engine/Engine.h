#pragma once

#include "NcEngine.h"
#include "assets/AssetServices.h"
#include "audio/AudioSystemImpl.h"
#include "camera/MainCameraImpl.h"
#include "ecs/EntityComponentSystem.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"
#include "physics/PhysicsSystemImpl.h"
#include "scene/SceneSystemImpl.h"
#include "task/Task.h"
#include "time/NcTime.h"
#include "ui/UISystemImpl.h"
#include "window/WindowImpl.h"

namespace nc
{
    class Engine final : public NcEngine
    {
        public:
            Engine(HINSTANCE hInstance, bool useEditorMode);
            ~Engine() noexcept;

            void Start(std::unique_ptr<Scene> initialScene) override;
            void Quit() noexcept override;
            void Shutdown() noexcept override;

            auto Audio() noexcept -> AudioSystem* override;
            auto MainCamera() noexcept -> nc::MainCamera* override;
            auto Registry() noexcept -> nc::Registry* override;
            auto Physics() noexcept -> PhysicsSystem* override;
            auto SceneSystem() noexcept -> nc::SceneSystem* override;
            auto UI() noexcept -> UISystem* override;

        private:
            camera::MainCameraImpl m_mainCamera;
            window::WindowImpl m_window;
            graphics::Graphics m_graphics;
            ecs::EntityComponentSystem m_ecs;
            physics::PhysicsSystemImpl m_physicsSystem;
            scene::SceneSystemImpl m_sceneSystem;
            time::Time m_time;
            audio::AudioSystemImpl m_audioSystem;
            ui::UISystemImpl m_uiSystem;
            tf::Executor m_taskExecutor;
            TaskGraph m_tasks;
            float m_dt;
            float m_frameDeltaTimeFactor;
            bool m_useEditorMode;
            bool m_isRunning;

            void BuildTaskGraph();
            void MainLoop();
            void EditorLoop();
            void DisableRunningFlag() noexcept;
            void ClearState();
            void DoSceneSwap();
            void FrameLogic(float dt);
            void FrameRender();
            void FrameCleanup();
            void SetBindings();
    };
} // end namespace nc::engine