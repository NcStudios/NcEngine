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
void SerializeMeshRenderer(std::ostream& stream, const graphics::MeshRenderer& out, const SerializationContext& ctx, const std::any&);
auto DeserializeMeshRenderer(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::MeshRenderer;
void SerializeParticleEmitter(std::ostream& stream, const graphics::ParticleEmitter& out, const SerializationContext& ctx, const std::any&);
auto DeserializeParticleEmitter(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::ParticleEmitter;
void SerializePointLight(std::ostream& stream, const graphics::PointLight& out, const SerializationContext& ctx, const std::any&);
auto DeserializePointLight(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::PointLight;
void SerializeSpotLight(std::ostream& stream, const graphics::SpotLight& out, const SerializationContext& ctx, const std::any&);
auto DeserializeSpotLight(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::SpotLight;
void SerializeToonRenderer(std::ostream& stream, const graphics::ToonRenderer& out, const SerializationContext& ctx, const std::any&);
auto DeserializeToonRenderer(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::ToonRenderer;
void SerializeRigidBody(std::ostream& stream, const physics::RigidBody& out, const SerializationContext&, const std::any&);
auto DeserializeRigidBody(std::istream& stream, const DeserializationContext&, const std::any&) -> physics::RigidBody;
} // namespace nc
