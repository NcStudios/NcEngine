#pragma once

#include "ncengine/NcFwd.h"

#include <any>

namespace nc::ui::editor
{
struct EditorContext;

void CollisionLogicUIWidget(CollisionLogic& logic, EditorContext&, const std::any&);
void FixedLogicUIWidget(FixedLogic& logic, EditorContext&, const std::any&);
void FrameLogicUIWidget(FrameLogic& logic, EditorContext&, const std::any&);
void TagUIWidget(Tag& tag, EditorContext&, const std::any&);
void TransformUIWidget(Transform& transform, EditorContext&, const std::any&);
void AudioSourceUIWidget(audio::AudioSource& audioSource, EditorContext&, const std::any&);
void MeshRendererUIWidget(graphics::MeshRenderer& meshRenderer, EditorContext&, const std::any&);
void ParticleEmitterUIWidget(graphics::ParticleEmitter& particleEmitter, EditorContext&, const std::any&);
void PointLightUIWidget(graphics::PointLight& pointLight, EditorContext&, const std::any&);
void SpotLightUIWidget(graphics::SpotLight& spotLight, EditorContext&, const std::any&);
void SkeletalAnimatorUIWidget(graphics::SkeletalAnimator& skeletalAnimator, EditorContext&, const std::any&);
void ToonRendererUIWidget(graphics::ToonRenderer& toonRenderer, EditorContext&, const std::any&);
void NetworkDispatcherUIWidget(net::NetworkDispatcher& networkDispatcher, EditorContext&, const std::any&);
void ColliderUIWidget(physics::Collider& collider, EditorContext&, const std::any&);
void ConcaveColliderUIWidget(physics::ConcaveCollider& concaveCollider, EditorContext&, const std::any&);
void OrientationClampUIWidget(physics::OrientationClamp& orientationClamp, EditorContext&, const std::any&);
void PhysicsBodyUIWidget(physics::PhysicsBody& physicsBody, EditorContext&, const std::any&);
void PhysicsMaterialUIWidget(physics::PhysicsMaterial& physicsMaterial, EditorContext&, const std::any&);
void PositionClampUIWidget(physics::PositionClamp& positionClamp, EditorContext&, const std::any&);
void VelocityRestrictionUIWidget(physics::VelocityRestriction& velocityRestriction, EditorContext&, const std::any&);
} // namespace nc::ui::editor
