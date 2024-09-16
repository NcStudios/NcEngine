#pragma once

#include "ncengine/NcFwd.h"

#include <any>

namespace nc
{
auto CreateAudioSource(Entity entity, const std::any&) -> audio::AudioSource;
auto CreateCollisionLogic(Entity entity, const std::any&) -> CollisionLogic;
auto CreateFixedLogic(Entity entity, const std::any&) -> FixedLogic;
auto CreateFrameLogic(Entity entity, const std::any&) -> FrameLogic;
auto CreateParticleEmitter(Entity entity, const std::any&) -> graphics::ParticleEmitter;
auto CreatePointLight(Entity entity, const std::any&) -> graphics::PointLight;
auto CreateMeshRenderer(Entity entity, const std::any&) -> graphics::MeshRenderer;
auto CreateToonRenderer(Entity entity, const std::any&) -> graphics::ToonRenderer;
auto CreateSkeletalAnimator(Entity entity, const std::any&) -> graphics::SkeletalAnimator;
auto CreateSpotLight(Entity entity, const std::any&) -> graphics::SpotLight;
auto CreateNetworkDispatcher(Entity entity, const std::any&) -> net::NetworkDispatcher;
auto CreateCollider(Entity entity, const std::any&) -> physics::Collider;
auto CreateOrientationClamp(Entity, const std::any&) -> physics::OrientationClamp;
auto CreatePhysicsBody(Entity entity, const std::any& userData) -> physics::PhysicsBody;
auto CreatePhysicsMaterial(Entity entity, const std::any&) -> physics::PhysicsMaterial;
auto CreatePositionClamp(Entity entity, const std::any&) -> physics::PositionClamp;
auto CreateVelocityRestriction(Entity entity, const std::any&) -> physics::VelocityRestriction;
auto CreateRigidBody(Entity entity, const std::any&) -> physics::RigidBody;
} // namespace nc
