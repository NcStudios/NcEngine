#pragma once

#include "Ecs.h"

namespace nc::editor
{
    /** Drawing of component widgets. */
    class Inspector
    {
        public:
            Inspector(registry_type* registry);
            void InspectEntity(Entity entity);

        private:
            registry_type* m_registry;

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