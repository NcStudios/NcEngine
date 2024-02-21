#pragma once

#include "ncengine/NcFwd.h"

#include <optional>

namespace nc
{
auto CreateAudioSource(Entity entity, void*) -> std::optional<audio::AudioSource>;
auto CreateCollisionLogic(Entity entity, void*) -> std::optional<CollisionLogic>;
auto CreateFixedLogic(Entity entity, void*) -> std::optional<FixedLogic>;
auto CreateFrameLogic(Entity entity, void*) -> std::optional<FrameLogic>;
auto CreateParticleEmitter(Entity entity, void*) -> std::optional<graphics::ParticleEmitter>;
auto CreatePointLight(Entity entity, void*) -> std::optional<graphics::PointLight>;
auto CreateMeshRenderer(Entity entity, void*) -> std::optional<graphics::MeshRenderer>;
auto CreateToonRenderer(Entity entity, void*) -> std::optional<graphics::ToonRenderer>;
auto CreateSkeletalAnimator(Entity entity, void*) -> std::optional<graphics::SkeletalAnimator>;
auto CreateNetworkDispatcher(Entity entity, void*) -> std::optional<net::NetworkDispatcher>;
auto CreateCollider(Entity entity, void*) -> std::optional<physics::Collider>;
auto CreateConcaveCollider(Entity entity, void*) -> std::optional<physics::ConcaveCollider>;
auto CreatePhysicsBody(Entity entity, void* userData) -> std::optional<physics::PhysicsBody>;
} // namespace nc
