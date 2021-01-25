#pragma once

#include "config/Config.h"
#include "win32/HInstanceForwardDecl.h"

#include <functional>
#include <memory>

namespace nc
{
    namespace camera { class MainCameraImpl; }
    namespace debug { class LogImpl; }
    namespace graphics 
    { 
        class Graphics;
        class FrameManager;
    }
    namespace engine { class Engine; }
    namespace ecs
    {
        class EcsImpl;
        class LightSystem;
        class RenderingSystem;
        class TransformSystem;
    }
    namespace physics { class PhysicsSystem; }
    namespace scene
    {
        class Scene;
        class SceneManagerImpl;
    }
    namespace time { class Timer; }
    namespace ui { class UIImpl; }
    namespace window { class WindowImpl; }
    class PointLightManager;
}

namespace nc::engine
{
    class EngineImpl
    {
        public:
            bool isRunning;

            EngineImpl(HINSTANCE hInstance, std::function<void(bool)> engineShutdownCallback);
            ~EngineImpl();
            void MainLoop(std::unique_ptr<scene::Scene> initialScene);
            void Shutdown();
            void ClearState();
            void DoSceneSwap();
            void FrameLogic(float dt);
            void FrameRender();
            void FrameCleanup();
            const config::Config& GetConfig() const;

        private:
            config::Config m_config;
            std::unique_ptr<debug::LogImpl> m_log;
            float m_frameDeltaTimeFactor;
            std::unique_ptr<window::WindowImpl> m_window;
            std::unique_ptr<graphics::Graphics> m_graphics;
            std::unique_ptr<physics::PhysicsSystem> m_physics;
            std::unique_ptr<ecs::EcsImpl> m_ecs;
            std::unique_ptr<ui::UIImpl> m_ui;
            std::unique_ptr<PointLightManager> m_pointLightManager;
            std::unique_ptr<scene::SceneManagerImpl> m_sceneManager;
            std::unique_ptr<graphics::FrameManager> m_frameManager;
            std::unique_ptr<camera::MainCameraImpl> m_mainCamera;

            void SetBindings();
    };
} // end namespace nc::engine