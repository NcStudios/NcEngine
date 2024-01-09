#pragma once

#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/window/Window.h"

namespace nc::sample
{
    /** Control camera by moving mouse to edge of the screen. */
    class EdgePanCamera : public graphics::Camera
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
            void OnResize(Vector2 screenExtent) override;

        private:
            Vector2 m_screenExtent;
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
            void Select(physics::ColliderType type);

        private:
            Entity m_currentObject;
            physics::ColliderType m_typeToSpawn;
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
    class Clickable : public FreeComponent, public physics::IClickable
    {
        public:
            Clickable(Entity self, std::string tag, physics::NcPhysics* physicsModule);
            ~Clickable() noexcept;
            void OnClick() override;

        private:
            std::string m_Tag;
            physics::NcPhysics* m_physicsModule;
    };

    /** Notifies the PhysicsSystem to perform a raycast against clickables 
     *  upon pressing the left mouse button. */
    class ClickHandler : public FreeComponent
    {
        public:
            ClickHandler(Entity self, physics::LayerMask mask, physics::NcPhysics* physicsModule);
            void Run(Entity self, Registry* registry, float dt);
            void SetLayer(physics::LayerMask mask);

        private:
            physics::LayerMask m_mask;
            physics::NcPhysics* m_physicsModule;
    };
}