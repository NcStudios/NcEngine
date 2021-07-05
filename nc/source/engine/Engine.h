#pragma once

#include "ecs/EntityComponentSystem.h"
#include "graphics/rendergraph/FrameManager.h"
#ifdef USE_VULKAN
    #include "graphics/vulkan/FrameManager.h"
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


#include "physics/DebugRenderer.h"


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
            graphics::vulkan::FrameManager m_frameManager2;
            // @todo: Implement missing managers below
        #else            
            graphics::Graphics m_graphics;
            PointLightManager m_pointLightManager;
            graphics::FrameManager m_frameManager;
        #endif
            ecs::EntityComponentSystem m_ecs;
            physics::PhysicsSystem m_physics;
            scene::SceneSystem m_sceneSystem;
            time::Time m_time;

        #ifndef USE_VULKAN
            ui::UIImpl m_ui;
        #endif

            physics::DebugRenderer m_debugRenderer;

            void ClearState();
            void DoSceneSwap();
            void FixedStepLogic(float dt);
            void FrameLogic(float dt);
            void FrameRender();
            void FrameCleanup();
            void SetBindings();
    };
} // end namespace nc::engine