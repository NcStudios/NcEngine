#include "ui/editor/ComponentWidgets.h"

namespace nc::editor
{
// If the editor is disabled, we need ui stubs because component registration
// references them in the handler.
void CollisionLogicUIWidget(CollisionLogic&) {}
void FixedLogicUIWidget(FixedLogic&) {}
void FrameLogicUIWidget(FrameLogic&) {}
void TagUIWidget(Tag&) {}
void TransformUIWidget(Transform&) {}
void AudioSourceUIWidget(audio::AudioSource&) {}
void MeshRendererUIWidget(graphics::MeshRenderer&) {}
void ParticleEmitterUIWidget(graphics::ParticleEmitter&) {}
void PointLightUIWidget(graphics::PointLight&) {}
void SkeletalAnimatorUIWidget(graphics::SkeletalAnimator&) {}
void ToonRendererUIWidget(graphics::ToonRenderer&) {}
void NetworkDispatcherUIWidget(net::NetworkDispatcher&) {}
void ColliderUIWidget(physics::Collider&) {}
void ConcaveColliderUIWidget(physics::ConcaveCollider&) {}
void PhysicsBodyUIWidget(physics::PhysicsBody&) {}
} // namespace nc::editor
