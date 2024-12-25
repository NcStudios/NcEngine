#pragma once

#include "ncengine/NcFwd.h"

#include <any>

namespace nc::ui::editor
{
struct EditorContext;

void FrameLogicUIWidget(FrameLogic& logic, EditorContext&, const std::any&);
void TagUIWidget(Tag& tag, EditorContext&, const std::any&);
void TransformUIWidget(Transform& transform, EditorContext&, const std::any&);
void AudioSourceUIWidget(audio::AudioSource& audioSource, EditorContext&, const std::any&);
void StaticMeshUIWidget(StaticMesh& meshRenderer, EditorContext&, const std::any&);
void ParticleEmitterUIWidget(ParticleEmitter& particleEmitter, EditorContext&, const std::any&);
void DirectionalLightUIWidget(DirectionalLight& directionalLight, EditorContext&, const std::any&);
void PointLightUIWidget(PointLight& pointLight, EditorContext&, const std::any&);
void SpotLightUIWidget(SpotLight& spotLight, EditorContext&, const std::any&);
void SkinnedMeshUIWidget(SkinnedMesh& skinnedMesh, EditorContext&, const std::any&);
void CollisionListenerUIWidget(CollisionListener& listener, EditorContext&, const std::any&);
void RigidBodyUIWidget(RigidBody& body, EditorContext&, const std::any&);
} // namespace nc::ui::editor
