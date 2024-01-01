#pragma once

#include "ncengine/NcFwd.h"

namespace nc
{
auto CreateAudioSource(Entity entity, void*) -> audio::AudioSource;
auto CreateCollisionLogic(Entity entity, void*) -> CollisionLogic;
auto CreateFixedLogic(Entity entity, void*) -> FixedLogic;
auto CreateFrameLogic(Entity entity, void*) -> FrameLogic;
auto CreateParticleEmitter(Entity entity, void*) -> graphics::ParticleEmitter;
auto CreateMeshRenderer(Entity entity, void*) -> graphics::MeshRenderer;
auto CreateToonRenderer(Entity entity, void*) -> graphics::ToonRenderer;
auto CreateSkeletalAnimator(Entity entity, void*) -> graphics::SkeletalAnimator;
auto CreateNetworkDispatcher(Entity entity, void*) -> net::NetworkDispatcher;
auto CreateCollider(Entity entity, void*) -> physics::Collider;
auto CreatePhysicsBody(Entity entity, void* userData) -> physics::PhysicsBody;
} // namespace nc
