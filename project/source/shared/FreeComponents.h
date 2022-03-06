#pragma once

#include "Window.h"
#include "physics/PhysicsModule.h"

#include "ecs/InvokeFreeComponent.h"
#include "ecs/Registry.h"
#include "ecs/component/All.h"

namespace nc::sample
{
    /** Control camera pan, look, and zoom with right mouse button, middle mouse button, and mouse wheel. */
    class SceneNavigationCamera : public Camera
    {
        public:
            SceneNavigationCamera(Entity entity, float panDamp, float lookDamp, float zoomDamp);
            void Run(Entity self, Registry* registry, float dt);

        private:
            Vector2 m_panPivot = Vector2::Zero();
            Vector2 m_lookPivot = Vector2::Zero();
            float m_zoom = 0.0f;
            float m_panDampen = 0.25f;
            float m_lookDampen = 0.005f;
            float m_zoomDampen = 1.4f;

            void Pan(float dt, Transform* transform);
            void Look(float dt, Transform* transform);
            void Zoom(float dt, Transform* transform);
    };

    /** Control camera by moving mouse to edge of the screen. */
    class EdgePanCamera : public Camera
    {
        public:
            EdgePanCamera(Entity entity);
            void Run(Entity self, Registry* registry, float dt);

        private:
            float m_lastFrameZoom;
            Vector3 m_lastFrameTranslation = Vector3::Zero();
    };

    /** Applies a constant rotation around an axis to the parent object */
    class ConstantRotation : public FreeComponent
    {
        public:
            ConstantRotation(Entity entity, Vector3 axis, float radiansPerSecond);
            void Run(Entity self, Registry* registry, float dt);

        private:
            Vector3 m_axis;
            float m_radiansPerSecond;
    };

    /** Applies a constant translation along a vector to the parent object. */
    class ConstantTranslation : public FreeComponent
    {
        public:
            ConstantTranslation(Entity entity, Vector3 velocity);
            void Run(Entity self, Registry* registry, float dt);

        private:
            Vector3 m_velocity;
    };

    /** Used by the Worms scene to make a point light track mouse movement. */
    class MouseFollower : public FreeComponent, public window::IOnResizeReceiver
    {
        public:
            MouseFollower(Entity entity);
            ~MouseFollower() noexcept;
            void Run(Entity self, Registry* registry, float dt);
            void OnResize(Vector2 screenDimensions) override;

        private:
            Vector2 m_screenDimensions;
            float m_viewPortDist;
            float m_zDepth;
            float m_zRatio;
    };

    /** Used by CollisionEvents scene to swap the shape of a moveable object. */
    class PrefabSelector : public FreeComponent
    {
        public:
            PrefabSelector(Entity entity);
            void Run(Entity self, Registry* registry, float);
            void Select(ColliderType type);

        private:
            Entity m_currentObject;
            ColliderType m_typeToSpawn;
            bool m_doSpawn;
    };

    /** Keeps track of FPS as seen by the logic components. This is not necessarily
     *  the number of frames being rendered. */
    class FPSTracker : public FreeComponent
    {
        public:
            FPSTracker(Entity entity);
            void Run(Entity self, Registry* registry, float dt);
            float GetFPS() const;

        private:
            unsigned m_frames = 0u;
            float m_seconds = 0.0f;
            float m_latestFPS = 0.0f;
    };

    /** Outputs to the game log when the parent object receives a click event. */
    class Clickable : public FreeComponent, public IClickable
    {
        public:
            Clickable(Entity entity, std::string tag, PhysicsModule* physicsModule);
            ~Clickable() noexcept;
            void OnClick() override;

        private:
            std::string m_Tag;
            PhysicsModule* m_physicsModule;
    };

    /** Notifies the PhysicsSystem to perform a raycast against clickables 
     *  upon pressing the left mouse button. */
    class ClickHandler : public FreeComponent
    {
        public:
            ClickHandler(Entity entity, LayerMask mask, PhysicsModule* physicsModule);
            void Run(Entity self, Registry* registry, float dt);
            void SetLayer(LayerMask mask);

        private:
            LayerMask m_mask;
            PhysicsModule* m_physicsModule;
    };
}