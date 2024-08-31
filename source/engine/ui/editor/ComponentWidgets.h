#pragma once

#include "ncengine/NcFwd.h"

#include <any>

namespace nc::ui::editor
{
struct EditorContext;

void CollisionLogicUIWidget(CollisionLogic& logic, Entity, EditorContext&, const std::any&);
void FixedLogicUIWidget(FixedLogic& logic, Entity, EditorContext&, const std::any&);
void FrameLogicUIWidget(FrameLogic& logic, Entity, EditorContext&, const std::any&);
void TagUIWidget(Tag& tag, Entity, EditorContext&, const std::any&);
void TransformUIWidget(Transform& transform, Entity, EditorContext&, const std::any&);
void AudioSourceUIWidget(audio::AudioSource& audioSource, Entity, EditorContext&, const std::any&);
void MeshRendererUIWidget(graphics::MeshRenderer& meshRenderer, Entity, EditorContext&, const std::any&);
void ParticleEmitterUIWidget(graphics::ParticleEmitter& particleEmitter, Entity, EditorContext&, const std::any&);
void PointLightUIWidget(graphics::PointLight& pointLight, Entity, EditorContext&, const std::any&);
void SpotLightUIWidget(graphics::SpotLight& spotLight, Entity, EditorContext&, const std::any&);
void SkeletalAnimatorUIWidget(graphics::SkeletalAnimator& skeletalAnimator, Entity, EditorContext&, const std::any&);
void ToonRendererUIWidget(graphics::ToonRenderer& toonRenderer, Entity, EditorContext&, const std::any&);
void NetworkDispatcherUIWidget(net::NetworkDispatcher& networkDispatcher, Entity, EditorContext&, const std::any&);
void ColliderUIWidget(physics::Collider& collider, Entity, EditorContext&, const std::any&);
void ConcaveColliderUIWidget(physics::ConcaveCollider& concaveCollider, Entity, EditorContext&, const std::any&);
void OrientationClampUIWidget(physics::OrientationClamp& orientationClamp, Entity, EditorContext&, const std::any&);
void PhysicsBodyUIWidget(physics::PhysicsBody& physicsBody, Entity, EditorContext&, const std::any&);
void PhysicsMaterialUIWidget(physics::PhysicsMaterial& physicsMaterial, Entity, EditorContext&, const std::any&);
void PositionClampUIWidget(physics::PositionClamp& positionClamp, Entity, EditorContext&, const std::any&);
void VelocityRestrictionUIWidget(physics::VelocityRestriction& velocityRestriction, Entity, EditorContext&, const std::any&);
} // namespace nc::ui::editor
