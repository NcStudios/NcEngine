#pragma once

#include "config/Config.h"
#include "win32/HInstanceForwardDecl.h"

#include <memory>

namespace nc
{
    namespace camera { class MainCamera; }
    namespace graphics { class Graphics; }
    namespace engine { class Engine; }
    namespace ecs
    {
        class EcsImpl;
        class LightSystem;
        class RenderingSystem;
        class TransformSystem;
    }
    namespace log { class Logger; }
    namespace physics { class PhysicsSystem; }
    namespace scene
    {
        class Scene;
        class SceneManager;
    }
    namespace time { class Timer; }
    namespace ui { class UISystem; }
    namespace window { class WindowImpl; }
}

namespace nc::engine
{
    class EngineImpl
    {
        public:
            bool isRunning;

            EngineImpl(HINSTANCE hInstance, Engine* topLevelEngine);
            ~EngineImpl();
            void MainLoop(std::unique_ptr<scene::Scene> initialScene);
            void Shutdown();
            void ClearState();
            void DoSceneSwap();
            void FrameLogic(float dt);
            void FrameRender();
            void FrameCleanup();

        private:

            config::Config m_config;
            float m_frameDeltaTimeFactor;
            std::unique_ptr<log::Logger> m_logger;
            std::unique_ptr<window::WindowImpl> m_window;
            std::unique_ptr<graphics::Graphics> m_graphics;
            std::unique_ptr<physics::PhysicsSystem> m_physics;
            std::unique_ptr<ecs::EcsImpl> m_ecs;
            std::unique_ptr<ui::UISystem> m_uiSystem;
            std::unique_ptr<scene::SceneManager> m_sceneManager;
            std::unique_ptr<camera::MainCamera> m_mainCamera;
            #ifdef NC_EDITOR_ENABLED
            std::unique_ptr<nc::time::Timer> m_frameLogicTimer;
            #endif
    };
} // end namespace nc::engine