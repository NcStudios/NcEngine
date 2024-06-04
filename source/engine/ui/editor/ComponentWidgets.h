#pragma once

#include "ncengine/NcFwd.h"

namespace nc::editor
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
void SpotLightUIWidget(graphics::SpotLight& spotLight);
void SkeletalAnimatorUIWidget(graphics::SkeletalAnimator& skeletalAnimator);
void ToonRendererUIWidget(graphics::ToonRenderer& toonRenderer);
void NetworkDispatcherUIWidget(net::NetworkDispatcher& networkDispatcher);
void ColliderUIWidget(physics::Collider& collider);
void ConcaveColliderUIWidget(physics::ConcaveCollider& concaveCollider);
void OrientationClampUIWidget(physics::OrientationClamp& orientationClamp);
void PhysicsBodyUIWidget(physics::PhysicsBody& physicsBody);
void PhysicsMaterialUIWidget(physics::PhysicsMaterial& physicsMaterial);
void PositionClampUIWidget(physics::PositionClamp& positionClamp);
void VelocityRestrictionUIWidget(physics::VelocityRestriction& velocityRestriction);
} // namespace nc::editor
