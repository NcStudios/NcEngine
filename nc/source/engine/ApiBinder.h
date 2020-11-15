#pragma once

namespace nc
{
    namespace camera { class MainCameraImpl; }
    namespace debug { class LogImpl; }
    namespace engine { class EngineImpl; }
    namespace physics { class PhysicsSystem; }
    namespace scene { class SceneManagerImpl; }
    namespace ui { class UIImpl; }
    namespace window { class WindowImpl; }
}

namespace nc::engine
{
    class ApiBinder
    {
        public:
            static void Bind(window::WindowImpl* window,
                             scene::SceneManagerImpl* sceneManager,
                             camera::MainCameraImpl* camera,
                             debug::LogImpl* log,
                             ui::UIImpl* ui,
                             EngineImpl* engineImpl,
                             physics::PhysicsSystem* physics);
    };
}