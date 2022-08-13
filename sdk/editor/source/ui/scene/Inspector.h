#pragma once

#include "ecs/Entity.h"

namespace nc
{
class AudioSource;
class Camera;
class ConcaveCollider;
class MeshRenderer;
class NetworkDispatcher;
class ParticleEmitter;
class PointLight;
class Registry;
class Transform;

namespace physics
{
class Collider;
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

            void DrawAudioSource(AudioSource* audioSource);
            void DrawCamera(Camera* camera);
            void DrawCollider(physics::Collider* collider);
            void DrawConcaveCollider(ConcaveCollider* concaveCollider);
            void DrawMeshRenderer(MeshRenderer* meshRenderer);
            void DrawNetworkDispatcher(NetworkDispatcher* networkDispatcher);
            void DrawParticleEmitter(ParticleEmitter* particleEmitter);
            void DrawPhysicsBody(physics::PhysicsBody* physicsBody);
            void DrawPointLight(PointLight* pointLight);
            void DrawTransform(Transform* transform);
            void DrawBoxColliderWidget(physics::Collider* collider);
            void DrawCapsuleColliderWidget(physics::Collider* collider);
            void DrawHullColliderWidget(physics::Collider* collider);
            void DrawSphereColliderWidget(physics::Collider* collider);
    };
}