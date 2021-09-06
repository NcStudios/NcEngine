#pragma once

#include "ecs/EntityComponentSystem.h"
#include "graphics/Graphics2.h"
#include "graphics/Renderer.h"
#include "job/JobSystem.h"
#include "physics/PhysicsSystem.h"
#include "scene/SceneSystem.h"
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
            bool m_isRunning;
            float m_frameDeltaTimeFactor;
            job::JobSystem m_jobSystem;
            window::WindowImpl m_window;
            graphics::Graphics2 m_graphics2;
            graphics::Renderer m_renderer;
            // @todo: Implement missing managers below
            ecs::EntityComponentSystem m_ecs;
            physics::PhysicsSystem m_physics;
            scene::SceneSystem m_sceneSystem;
            time::Time m_time;
            ui::UIImpl m_ui;

            void ClearState();
            void DoSceneSwap();
            void FixedStepLogic(float dt);
            void FrameLogic(float dt);
            void FrameRender();
            void FrameCleanup();
            void SetBindings();
    };
} // end namespace nc::engine