#include "ui/editor/ComponentWidgets.h"
#include "ncengine/ecs/Entity.h"

namespace nc::ui::editor
{
struct EditorContext;

// If the editor is disabled, we need ui stubs because component registration
// references them in the handler.
void CollisionLogicUIWidget(CollisionLogic&, Entity, EditorContext&, const std::any&) {}
void FixedLogicUIWidget(FixedLogic&, Entity, EditorContext&, const std::any&) {}
void FrameLogicUIWidget(FrameLogic&, Entity, EditorContext&, const std::any&) {}
void TagUIWidget(Tag&, Entity, EditorContext&, const std::any&) {}
void TransformUIWidget(Transform&, Entity, EditorContext&, const std::any&) {}
void AudioSourceUIWidget(audio::AudioSource&, Entity, EditorContext&, const std::any&) {}
void MeshRendererUIWidget(graphics::MeshRenderer&, Entity, EditorContext&, const std::any&) {}
void ParticleEmitterUIWidget(graphics::ParticleEmitter&, Entity, EditorContext&, const std::any&) {}
void PointLightUIWidget(graphics::PointLight&, Entity, EditorContext&, const std::any&) {}
void SpotLightUIWidget(graphics::SpotLight&, Entity, EditorContext&, const std::any&) {}
void SkeletalAnimatorUIWidget(graphics::SkeletalAnimator&, Entity, EditorContext&, const std::any&) {}
void ToonRendererUIWidget(graphics::ToonRenderer&, Entity, EditorContext&, const std::any&) {}
void NetworkDispatcherUIWidget(net::NetworkDispatcher&, Entity, EditorContext&, const std::any&) {}
void ColliderUIWidget(physics::Collider&, Entity, EditorContext&, const std::any&) {}
void ConcaveColliderUIWidget(physics::ConcaveCollider&, Entity, EditorContext&, const std::any&) {}
void OrientationClampUIWidget(physics::OrientationClamp&, Entity, EditorContext&, const std::any&) {}
void PhysicsBodyUIWidget(physics::PhysicsBody&, Entity, EditorContext&, const std::any&) {}
void PhysicsMaterialUIWidget(physics::PhysicsMaterial&, Entity, EditorContext&, const std::any&) {}
void PositionClampUIWidget(physics::PositionClamp&, Entity, EditorContext&, const std::any&) {}
void VelocityRestrictionUIWidget(physics::VelocityRestriction&, Entity, EditorContext&, const std::any&) {}
} // namespace nc::ui::editor
