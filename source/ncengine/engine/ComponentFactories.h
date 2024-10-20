#pragma once

#include "ncengine/NcFwd.h"

#include <any>

namespace nc
{
auto CreateAudioSource(Entity entity, const std::any&) -> audio::AudioSource;
auto CreateFrameLogic(Entity entity, const std::any&) -> FrameLogic;
auto CreateParticleEmitter(Entity entity, const std::any&) -> graphics::ParticleEmitter;
auto CreatePointLight(Entity entity, const std::any&) -> graphics::PointLight;
auto CreateMeshRenderer(Entity entity, const std::any&) -> graphics::MeshRenderer;
auto CreateToonRenderer(Entity entity, const std::any&) -> graphics::ToonRenderer;
auto CreateSkeletalAnimator(Entity entity, const std::any&) -> graphics::SkeletalAnimator;
auto CreateSpotLight(Entity entity, const std::any&) -> graphics::SpotLight;
auto CreateNetworkDispatcher(Entity entity, const std::any&) -> net::NetworkDispatcher;
auto CreateRigidBody(Entity entity, const std::any&) -> RigidBody;
} // namespace nc
