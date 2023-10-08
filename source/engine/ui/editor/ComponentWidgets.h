#pragma once

namespace nc
{
class CollisionLogic;
class FixedLogic;
class FrameLogic;
class Tag;
class Transform;

namespace audio
{
class AudioSource;
} // namespace audio

namespace graphics
{
class MeshRenderer;
class ParticleEmitter;
class PointLight;
class ToonRenderer;
} // namespace graphics

namespace net
{
class NetworkDispatcher;
} // namespace net

namespace physics
{
class Collider;
class ConcaveCollider;
class PhysicsBody;
} // namespace physics

namespace editor
{
void CollisionLogicUIWidget(CollisionLogic& logic);
void FixedLogicUIWidget(FixedLogic& logic);
void FrameLogicUIWidget(FrameLogic& logic);
void TagUIWidget(Tag& tag);
void TransformUIWidget(Transform& transform);
void AudioSourceUIWidget(audio::AudioSource& audioSource);
void MeshRendererUIWidget(graphics::MeshRenderer& meshRenderer);
void ParticleEmitterUIWidget(graphics::ParticleEmitter& particleEmitter);
void PointLightUIWidget(graphics::PointLight& pointLight);
void ToonRendererUIWidget(graphics::ToonRenderer& toonRenderer);
void NetworkDispatcherUIWidget(net::NetworkDispatcher& networkDispatcher);
void ColliderUIWidget(physics::Collider& collider);
void ConcaveColliderUIWidget(physics::ConcaveCollider& concaveCollider);
void PhysicsBodyUIWidget(physics::PhysicsBody& physicsBody);
} // namespace editor
} // namespace nc
