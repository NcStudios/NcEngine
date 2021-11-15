#pragma once

#include "ecs/Entity.h"

namespace nc
{
    class AudioSource;
    class Collider;
    class ConcaveCollider;
    class MeshRenderer;
    class NetworkDispatcher;
    class ParticleEmitter;
    class PhysicsBody;
    class PointLight;
    class Registry;
    class Transform;
}

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
            void DrawCollider(Collider* collider);
            void DrawConcaveCollider(ConcaveCollider* concaveCollider);
            void DrawMeshRenderer(MeshRenderer* meshRenderer);
            void DrawNetworkDispatcher(NetworkDispatcher* networkDispatcher);
            void DrawParticleEmitter(ParticleEmitter* particleEmitter);
            void DrawPhysicsBody(PhysicsBody* physicsBody);
            void DrawPointLight(PointLight* pointLight);
            void DrawTransform(Transform* transform);
    };
}