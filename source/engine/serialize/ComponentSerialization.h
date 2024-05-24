#pragma once

#include "ncengine/NcFwd.h"

#include <any>
#include <iosfwd>

namespace nc
{
struct SerializationContext;
struct DeserializationContext;

void SerializeAudioSource(std::ostream& stream, const audio::AudioSource& out, const SerializationContext& ctx, const std::any&);
auto DeserializeAudioSource(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> audio::AudioSource;
void SerializeCollider(std::ostream& stream, const physics::Collider& out, const SerializationContext& ctx, const std::any&);
auto DeserializeCollider(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> physics::Collider;
void SerializeConcaveCollider(std::ostream& stream, const physics::ConcaveCollider& out, const SerializationContext& ctx, const std::any&);
auto DeserializeConcaveCollider(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> physics::ConcaveCollider;
void SerializeMeshRenderer(std::ostream& stream, const graphics::MeshRenderer& out, const SerializationContext& ctx, const std::any&);
auto DeserializeMeshRenderer(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::MeshRenderer;
void SerializeParticleEmitter(std::ostream& stream, const graphics::ParticleEmitter& out, const SerializationContext& ctx, const std::any&);
auto DeserializeParticleEmitter(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::ParticleEmitter;
void SerializePhysicsBody(std::ostream& stream, const physics::PhysicsBody& out, const SerializationContext& ctx, const std::any&);
auto DeserializePhysicsBody(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> physics::PhysicsBody;
void SerializePhysicsMaterial(std::ostream& stream, const physics::PhysicsMaterial& out, const SerializationContext& ctx, const std::any&);
auto DeserializePhysicsMaterial(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> physics::PhysicsMaterial;
void SerializePointLight(std::ostream& stream, const graphics::PointLight& out, const SerializationContext& ctx, const std::any&);
auto DeserializePointLight(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::PointLight;
void SerializePositionClamp(std::ostream& stream, const physics::PositionClamp& out, const SerializationContext&, const std::any&);
auto DeserializePositionClamp(std::istream& stream, const DeserializationContext&, const std::any&) -> physics::PositionClamp;
void SerializeSpotLight(std::ostream& stream, const graphics::SpotLight& out, const SerializationContext& ctx, const std::any&);
auto DeserializeSpotLight(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::SpotLight;
void SerializeToonRenderer(std::ostream& stream, const graphics::ToonRenderer& out, const SerializationContext& ctx, const std::any&);
auto DeserializeToonRenderer(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::ToonRenderer;
void SerializeVelocityRestriction(std::ostream& stream, const physics::VelocityRestriction& out, const SerializationContext&, const std::any&);
auto DeserializeVelocityRestriction(std::istream& stream, const DeserializationContext&, const std::any&) -> physics::VelocityRestriction;
} // namespace nc
