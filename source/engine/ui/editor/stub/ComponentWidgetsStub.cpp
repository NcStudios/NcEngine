#include "ui/editor/ComponentWidgets.h"
#include "ncengine/ecs/Entity.h"

namespace nc::ui::editor
{
struct EditorContext;

// If the editor is disabled, we need ui stubs because component registration
// references them in the handler.
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
void CollisionListenerUIWidget(physics::CollisionListener&, EditorContext&, const std::any&) {}
void RigidBodyUIWidget(physics::RigidBody&, EditorContext&, const std::any&) {}
} // namespace nc::ui::editor
