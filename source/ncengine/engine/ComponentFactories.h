#pragma once

#include "ncengine/NcFwd.h"

#include <any>

namespace nc
{
auto CreateAudioSource(Entity entity, const std::any&) -> AudioSource;
auto CreateFrameLogic(Entity entity, const std::any&) -> FrameLogic;
auto CreateParticleEmitter(Entity entity, const std::any&) -> ParticleEmitter;
auto CreatePointLight(Entity entity, const std::any&) -> PointLight;
auto CreateSkinnedMesh(Entity entity, const std::any&) -> SkinnedMesh;
auto CreateStaticMesh(Entity entity, const std::any&) -> StaticMesh;
auto CreateSpotLight(Entity entity, const std::any&) -> SpotLight;
auto CreateRigidBody(Entity entity, const std::any&) -> RigidBody;
auto CreateDirectionalLight(Entity entity, const std::any&) -> DirectionalLight;
} // namespace nc
