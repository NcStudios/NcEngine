#include "ui/editor/ComponentWidgets.h"
#include "ncengine/ecs/Entity.h"

namespace nc::ui::editor
{
struct EditorContext;

// If the editor is disabled, we need ui stubs because component registration
// references them in the handler.
void CollisionLogicUIWidget(CollisionLogic&, EditorContext&, const std::any&) {}
void FixedLogicUIWidget(FixedLogic&, EditorContext&, const std::any&) {}
void FrameLogicUIWidget(FrameLogic&, EditorContext&, const std::any&) {}
void TagUIWidget(Tag&, EditorContext&, const std::any&) {}
void TransformUIWidget(Transform&, EditorContext&, const std::any&) {}
void AudioSourceUIWidget(audio::AudioSource&, EditorContext&, const std::any&) {}
void MeshRendererUIWidget(graphics::MeshRenderer&, EditorContext&, const std::any&) {}
void ParticleEmitterUIWidget(graphics::ParticleEmitter&, EditorContext&, const std::any&) {}
void PointLightUIWidget(graphics::PointLight&, EditorContext&, const std::any&) {}
void SpotLightUIWidget(graphics::SpotLight&, EditorContext&, const std::any&) {}
void SkeletalAnimatorUIWidget(graphics::SkeletalAnimator&, EditorContext&, const std::any&) {}
void ToonRendererUIWidget(graphics::ToonRenderer&, EditorContext&, const std::any&) {}
void NetworkDispatcherUIWidget(net::NetworkDispatcher&, EditorContext&, const std::any&) {}
void ColliderUIWidget(physics::Collider&, EditorContext&, const std::any&) {}
void ConcaveColliderUIWidget(physics::ConcaveCollider&, EditorContext&, const std::any&) {}
void OrientationClampUIWidget(physics::OrientationClamp&, EditorContext&, const std::any&) {}
void PhysicsBodyUIWidget(physics::PhysicsBody&, EditorContext&, const std::any&) {}
void PhysicsMaterialUIWidget(physics::PhysicsMaterial&, EditorContext&, const std::any&) {}
void PositionClampUIWidget(physics::PositionClamp&, EditorContext&, const std::any&) {}
void VelocityRestrictionUIWidget(physics::VelocityRestriction&, EditorContext&, const std::any&) {}
void CollisionListenerUIWidget(physics::CollisionListener&, EditorContext&, const std::any&) {}
void RigidBodyUIWidget(physics::RigidBody&, EditorContext&, const std::any&) {}
} // namespace nc::ui::editor
