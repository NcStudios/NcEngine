#pragma once

#include "ncengine/NcFwd.h"

namespace nc::ui::editor
{
struct EditorContext;

void CollisionLogicUIWidget(CollisionLogic& logic, Entity, EditorContext&);
void FixedLogicUIWidget(FixedLogic& logic, Entity, EditorContext&);
void FrameLogicUIWidget(FrameLogic& logic, Entity, EditorContext&);
void TagUIWidget(Tag& tag, Entity, EditorContext&);
void TransformUIWidget(Transform& transform, Entity, EditorContext&);
void AudioSourceUIWidget(audio::AudioSource& audioSource, Entity, EditorContext&);
void MeshRendererUIWidget(graphics::MeshRenderer& meshRenderer, Entity, EditorContext&);
void ParticleEmitterUIWidget(graphics::ParticleEmitter& particleEmitter, Entity, EditorContext&);
void PointLightUIWidget(graphics::PointLight& pointLight, Entity, EditorContext&);
void SpotLightUIWidget(graphics::SpotLight& spotLight, Entity, EditorContext&);
void SkeletalAnimatorUIWidget(graphics::SkeletalAnimator& skeletalAnimator, Entity, EditorContext&);
void ToonRendererUIWidget(graphics::ToonRenderer& toonRenderer, Entity, EditorContext&);
void NetworkDispatcherUIWidget(net::NetworkDispatcher& networkDispatcher, Entity, EditorContext&);
void ColliderUIWidget(physics::Collider& collider, Entity, EditorContext&);
void ConcaveColliderUIWidget(physics::ConcaveCollider& concaveCollider, Entity, EditorContext&);
void OrientationClampUIWidget(physics::OrientationClamp& orientationClamp, Entity, EditorContext&);
void PhysicsBodyUIWidget(physics::PhysicsBody& physicsBody, Entity, EditorContext&);
void PhysicsMaterialUIWidget(physics::PhysicsMaterial& physicsMaterial, Entity, EditorContext&);
void PositionClampUIWidget(physics::PositionClamp& positionClamp, Entity, EditorContext&);
void VelocityRestrictionUIWidget(physics::VelocityRestriction& velocityRestriction, Entity, EditorContext&);
} // namespace nc::ui::editor
