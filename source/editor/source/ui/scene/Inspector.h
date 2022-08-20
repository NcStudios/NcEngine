#pragma once

#include "ecs/Entity.h"

namespace nc
{
class Registry;
class Transform;

namespace audio
{
class AudioSource;
}

namespace graphics
{
class Camera;
class MeshRenderer;
class ParticleEmitter;
class PointLight;
}

namespace net
{
class NetworkDispatcher;
}

namespace physics
{
class Collider;
class ConcaveCollider;
class PhysicsBody;
}
} // namespace nc

namespace nc::editor
{
    class AssetManifest;

    /** Drawing of component widgets. */
    class Inspector
    {
        public:
            Inspector(Registry* registry, AssetManifest* assetManifest);
            void InspectEntity(Entity entity);

        private:
            Registry* m_registry;
            AssetManifest* m_assetManifest;

            void DrawAudioSource(audio::AudioSource* audioSource);
            void DrawCamera(graphics::Camera* camera);
            void DrawCollider(physics::Collider* collider);
            void DrawConcaveCollider(physics::ConcaveCollider* concaveCollider);
            void DrawMeshRenderer(graphics::MeshRenderer* meshRenderer);
            void DrawNetworkDispatcher(net::NetworkDispatcher* networkDispatcher);
            void DrawParticleEmitter(graphics::ParticleEmitter* particleEmitter);
            void DrawPhysicsBody(physics::PhysicsBody* physicsBody);
            void DrawPointLight(graphics::PointLight* pointLight);
            void DrawTransform(Transform* transform);
            void DrawBoxColliderWidget(physics::Collider* collider);
            void DrawCapsuleColliderWidget(physics::Collider* collider);
            void DrawHullColliderWidget(physics::Collider* collider);
            void DrawSphereColliderWidget(physics::Collider* collider);
    };
}