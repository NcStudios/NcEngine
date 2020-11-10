#pragma once

#include "config/Config.h"
#include "win32/HInstanceForwardDecl.h"

#include <memory>

namespace nc
{
    namespace camera { class MainCamera; }
    namespace graphics { class Graphics; }
    namespace ecs
    {
        class ECSImpl;
        class LightSystem;
        class RenderingSystem;
        class TransformSystem;
    }
    namespace log { class Logger; }
    namespace physics { class PhysicsSystem; }
    namespace scene { class SceneManager; }
    namespace time { class Timer; }
    namespace ui { class UISystem; }
    namespace window { class WindowImpl; }
}

namespace nc::engine
{
class Engine;

class EngineImpl
{
    public:
        bool isRunning;

        EngineImpl(HINSTANCE hInstance, Engine* topLevelEngine);
        ~EngineImpl();
        void MainLoop();
        void Shutdown();
        void ClearState();
        void DoSceneSwap();
        void FrameLogic(float dt);
        void FrameRender(float dt);
        void FrameCleanup();

    private:

        config::Config m_config;
        float m_frameDeltaTimeFactor;
        std::unique_ptr<log::Logger> m_logger;
        std::unique_ptr<window::WindowImpl> m_window;
        std::unique_ptr<ecs::TransformSystem> m_transform;
        std::unique_ptr<ecs::RenderingSystem> m_rendering;
        std::unique_ptr<ecs::LightSystem> m_light;
        std::unique_ptr<physics::PhysicsSystem> m_physics;
        std::unique_ptr<ecs::ECSImpl> m_ecs;
        std::unique_ptr<ui::UISystem> m_uiSystem;
        std::unique_ptr<scene::SceneManager> m_sceneManager;
        std::unique_ptr<camera::MainCamera> m_mainCamera;
        #ifdef NC_EDITOR_ENABLED
        std::unique_ptr<nc::time::Timer> m_frameLogicTimer;
        #endif
};
} // end namespace nc::engine