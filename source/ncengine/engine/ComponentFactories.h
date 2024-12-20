#pragma once

#include "ncengine/NcFwd.h"

#include <any>

namespace nc
{
auto CreateAudioSource(Entity entity, const std::any&) -> audio::AudioSource;
auto CreateFrameLogic(Entity entity, const std::any&) -> FrameLogic;
auto CreateParticleEmitter(Entity entity, const std::any&) -> graphics::ParticleEmitter;
auto CreatePointLight(Entity entity, const std::any&) -> graphics::PointLight;
auto CreateSkinnedMesh(Entity entity, const std::any&) -> SkinnedMesh;
auto CreateStaticMesh(Entity entity, const std::any&) -> StaticMesh;
auto CreateSpotLight(Entity entity, const std::any&) -> graphics::SpotLight;
auto CreateRigidBody(Entity entity, const std::any&) -> RigidBody;
auto CreateDirectionalLight(Entity entity, const std::any&) -> graphics::DirectionalLight;
} // namespace nc
