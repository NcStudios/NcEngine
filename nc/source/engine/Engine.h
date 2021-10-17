#pragma once

#include "assets/AssetService.h"
#include "assets/AssetServices.h"
#include "audio/AudioSystem.h"
#include "ecs/EntityComponentSystem.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"
#include "physics/PhysicsSystem.h"
#include "scene/SceneSystem.h"
#include "task/Task.h"
#include "time/NcTime.h"
#include "ui/UIImpl.h"
#include "window/WindowImpl.h"

namespace nc::core
{
    class Engine
    {
        public:
            Engine(HINSTANCE hInstance);
            void DisableRunningFlag();
            void MainLoop(std::unique_ptr<scene::Scene> initialScene);
            void Shutdown();

        private:
            window::WindowImpl m_window;
            graphics::Graphics m_graphics;
            AssetServices m_assetServices;
            ecs::EntityComponentSystem m_ecs;
            physics::PhysicsSystem m_physics;
            scene::SceneSystem m_sceneSystem;
            time::Time m_time;
            audio::AudioSystem m_audioSystem;
            ui::UIImpl m_ui;
            tf::Executor m_taskExecutor;
            TaskGraph m_tasks;
            float m_dt;
            float m_frameDeltaTimeFactor;
            uint32_t m_currentImageIndex;
            bool m_isRunning;

            void BuildTaskGraph();
            void ClearState();
            void DoSceneSwap();
            void FrameLogic(float dt);
            void FrameRender();
            void FrameCleanup();
            void SetBindings();
    };
} // end namespace nc::engine