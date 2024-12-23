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
void SerializeDirectionalLight(std::ostream& stream, const DirectionalLight& out, const SerializationContext& ctx, const std::any&);
auto DeserializeDirectionalLight(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> DirectionalLight;
void SerializeParticleEmitter(std::ostream& stream, const graphics::ParticleEmitter& out, const SerializationContext& ctx, const std::any&);
auto DeserializeParticleEmitter(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::ParticleEmitter;
void SerializePointLight(std::ostream& stream, const PointLight& out, const SerializationContext& ctx, const std::any&);
auto DeserializePointLight(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> PointLight;
void SerializeSpotLight(std::ostream& stream, const SpotLight& out, const SerializationContext& ctx, const std::any&);
auto DeserializeSpotLight(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> SpotLight;
void SerializeRigidBody(std::ostream& stream, const RigidBody& out, const SerializationContext&, const std::any&);
auto DeserializeRigidBody(std::istream& stream, const DeserializationContext&, const std::any&) -> RigidBody;
} // namespace nc
