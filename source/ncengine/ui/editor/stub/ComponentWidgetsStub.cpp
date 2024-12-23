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
void DirectionalLightUIWidget(DirectionalLight&, EditorContext&, const std::any&) {}
void StaticMeshUIWidget(StaticMesh&, EditorContext&, const std::any&);
void ParticleEmitterUIWidget(graphics::ParticleEmitter&, EditorContext&, const std::any&) {}
void PointLightUIWidget(PointLight&, EditorContext&, const std::any&) {}
void SpotLightUIWidget(SpotLight&, EditorContext&, const std::any&) {}
void SkinnedMeshUIWidget(SkinnedMesh&, EditorContext&, const std::any&) {}
void CollisionListenerUIWidget(CollisionListener&, EditorContext&, const std::any&) {}
void RigidBodyUIWidget(RigidBody&, EditorContext&, const std::any&) {}
} // namespace nc::ui::editor
