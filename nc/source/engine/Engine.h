#pragma once

#include "ecs/EntityComponentSystem.h"
#include "graphics/rendergraph/FrameManager.h"
#ifdef USE_VULKAN
    #include "graphics/Graphics2.h"
#else
    #include "graphics/Graphics.h"
#endif
#include "job/JobSystem.h"
#include "physics/PhysicsSystem.h"
#include "component/PointLightManager.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
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
        #ifdef USE_VULKAN
            graphics::Graphics2 m_graphics2;
            // @todo: Implement missing managers below
        #else            
            graphics::Graphics m_graphics;
            PointLightManager m_pointLightManager;
            graphics::FrameManager m_frameManager;
        #endif
            physics::PhysicsSystem m_physics;
            ecs::EntityComponentSystem m_ecs;
            scene::SceneSystem m_sceneSystem;
            time::Time m_time;

        #ifndef USE_VULKAN
            ui::UIImpl m_ui;
        #endif

            void ClearState();
            void DoSceneSwap();
            void FixedStepLogic();
            void FrameLogic(float dt);
            void FrameRender();
            void FrameCleanup();
            void SetBindings();
    };
} // end namespace nc::engine