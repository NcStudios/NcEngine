#pragma once

#include "ncengine/NcFwd.h"

#include <iosfwd>

namespace nc
{
struct SerializationContext;
struct DeserializationContext;

void SerializeAudioSource(std::ostream& stream, const audio::AudioSource& out, const SerializationContext& ctx, void*);
auto DeserializeAudioSource(std::istream& stream, const DeserializationContext& ctx, void*) -> audio::AudioSource;
void SerializeCollider(std::ostream& stream, const physics::Collider& out, const SerializationContext& ctx, void*);
auto DeserializeCollider(std::istream& stream, const DeserializationContext& ctx, void*) -> physics::Collider;
void SerializeConcaveCollider(std::ostream& stream, const physics::ConcaveCollider& out, const SerializationContext& ctx, void*);
auto DeserializeConcaveCollider(std::istream& stream, const DeserializationContext& ctx, void*) -> physics::ConcaveCollider;
void SerializeMeshRenderer(std::ostream& stream, const graphics::MeshRenderer& out, const SerializationContext& ctx, void*);
auto DeserializeMeshRenderer(std::istream& stream, const DeserializationContext& ctx, void*) -> graphics::MeshRenderer;
void SerializeParticleEmitter(std::ostream& stream, const graphics::ParticleEmitter& out, const SerializationContext& ctx, void*);
auto DeserializeParticleEmitter(std::istream& stream, const DeserializationContext& ctx, void*) -> graphics::ParticleEmitter;
void SerializePhysicsBody(std::ostream& stream, const physics::PhysicsBody& out, const SerializationContext& ctx, void*);
auto DeserializePhysicsBody(std::istream& stream, const DeserializationContext& ctx, void*) -> physics::PhysicsBody;
void SerializePhysicsMaterial(std::ostream& stream, const physics::PhysicsMaterial& out, const SerializationContext& ctx, void*);
auto DeserializePhysicsMaterial(std::istream& stream, const DeserializationContext& ctx, void*) -> physics::PhysicsMaterial;
void SerializePointLight(std::ostream& stream, const graphics::PointLight& out, const SerializationContext& ctx, void*);
auto DeserializePointLight(std::istream& stream, const DeserializationContext& ctx, void*) -> graphics::PointLight;
void SerializePositionClamp(std::ostream& stream, const physics::PositionClamp& out, const SerializationContext&, void*);
auto DeserializePositionClamp(std::istream& stream, const DeserializationContext&, void*) -> physics::PositionClamp;
void SerializeToonRenderer(std::ostream& stream, const graphics::ToonRenderer& out, const SerializationContext& ctx, void*);
auto DeserializeToonRenderer(std::istream& stream, const DeserializationContext& ctx, void*) -> graphics::ToonRenderer;
void SerializeVelocityRestriction(std::ostream& stream, const physics::VelocityRestriction& out, const SerializationContext&, void*);
auto DeserializeVelocityRestriction(std::istream& stream, const DeserializationContext&, void*) -> physics::VelocityRestriction;
} // namespace nc
