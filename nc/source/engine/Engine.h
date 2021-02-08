#pragma once

#include "ecs/EntityComponentSystem.h"
#include "graphics/rendergraph/FrameManager.h"
#ifdef USE_VULKAN
    #include "graphics/Graphics2.h"
#else
    #include "graphics/Graphics.h"
#endif
#include "physics/PhysicsSystem.h"
#include "component/PointLightManager.h"
#include "scene/SceneSystem.h"
#include "time/NcTime.h"
#include "ui/UIImpl.h"
#include "window/WindowImpl.h"

#include <functional>

namespace nc::core
{
    class Engine
    {
        public:
            Engine(HINSTANCE hInstance);
            ~Engine();
            void DisableRunningFlag();
            void MainLoop(std::unique_ptr<scene::Scene> initialScene);
            void Shutdown();

        private:
            bool m_isRunning;
            float m_frameDeltaTimeFactor;
            window::WindowImpl m_window;
        #ifdef USE_VULKAN
            graphics::Graphics2 m_graphics2;
            // @todo: Implement missing managers below
        #else            
            graphics::Graphics m_graphics;
            ui::UIImpl m_ui;
            PointLightManager m_pointLightManager;
            graphics::FrameManager m_frameManager;
        #endif
            physics::PhysicsSystem m_physics;
            ecs::EntityComponentSystem m_ecs;
            scene::SceneSystem m_sceneSystem;
            time::Time m_time;

            void ClearState();
            void DoSceneSwap();
            void FixedStepLogic();
            void FrameLogic(float dt);
            void FrameRender();
            void FrameCleanup();
            void SetBindings();
    };
} // end namespace nc::engine