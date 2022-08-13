#pragma once

#include "window/Window.h"
#include "physics/PhysicsModule.h"

#include "ecs/InvokeFreeComponent.h"
#include "ecs/Registry.h"
#include "ecs/component/All.h"

namespace nc::sample
{
    /** Control camera by moving mouse to edge of the screen. */
    class EdgePanCamera : public Camera
    {
        public:
            EdgePanCamera(Entity self);
            void Run(Entity self, Registry* registry, float dt);

        private:
            Vector3 m_lastFrameTranslation = Vector3::Zero();
            float m_lastFrameZoom = 0.0f;
    };

    /** Applies a constant rotation around an axis to the parent object */
    class ConstantRotation : public FreeComponent
    {
        public:
            ConstantRotation(Entity self, Vector3 axis, float radiansPerSecond);
            void Run(Entity self, Registry* registry, float dt);

        private:
            Vector3 m_axis = Vector3::Zero();
            float m_radiansPerSecond = 0.0f;
    };

    /** Applies a constant translation along a vector to the parent object. */
    class ConstantTranslation : public FreeComponent
    {
        public:
            ConstantTranslation(Entity self, Vector3 velocity);
            void Run(Entity self, Registry* registry, float dt);

        private:
            Vector3 m_velocity = Vector3::Zero();
    };

    /** Used by the Worms scene to make a point light track mouse movement. */
    class MouseFollower : public FreeComponent, public window::IOnResizeReceiver
    {
        public:
            MouseFollower(Entity self);
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
            PrefabSelector(Entity self);
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
            FPSTracker(Entity self);
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
            Clickable(Entity self, std::string tag, PhysicsModule* physicsModule);
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
            ClickHandler(Entity self, LayerMask mask, PhysicsModule* physicsModule);
            void Run(Entity self, Registry* registry, float dt);
            void SetLayer(LayerMask mask);

        private:
            LayerMask m_mask;
            PhysicsModule* m_physicsModule;
    };
}