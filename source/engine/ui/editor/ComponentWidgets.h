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
void MeshRendererUIWidget(graphics::MeshRenderer& meshRenderer, EditorContext&, const std::any&);
void ParticleEmitterUIWidget(graphics::ParticleEmitter& particleEmitter, EditorContext&, const std::any&);
void PointLightUIWidget(graphics::PointLight& pointLight, EditorContext&, const std::any&);
void SpotLightUIWidget(graphics::SpotLight& spotLight, EditorContext&, const std::any&);
void SkeletalAnimatorUIWidget(graphics::SkeletalAnimator& skeletalAnimator, EditorContext&, const std::any&);
void ToonRendererUIWidget(graphics::ToonRenderer& toonRenderer, EditorContext&, const std::any&);
void NetworkDispatcherUIWidget(net::NetworkDispatcher& networkDispatcher, EditorContext&, const std::any&);
void CollisionListenerUIWidget(CollisionListener& listener, EditorContext&, const std::any&);
void RigidBodyUIWidget(RigidBody& body, EditorContext&, const std::any&);
} // namespace nc::ui::editor
