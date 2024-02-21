#include "ComponentSerialization.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/serialize/SceneSerialization.h"
#include "physics/ColliderUtility.h"

#include "ncutility/BinarySerialization.h"

namespace nc
{
void SerializeAudioSource(std::ostream& stream, const audio::AudioSource& out, const SerializationContext& ctx, void*)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.GetAssetPaths());
    serialize::Serialize(stream, out.GetProperties());
}

auto DeserializeAudioSource(std::istream& stream, const DeserializationContext& ctx, void*) -> audio::AudioSource
{
    auto id = uint32_t{};
    auto paths = std::vector<std::string>{};
    auto properties = audio::AudioSourceProperties{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, paths);
    serialize::Deserialize(stream, properties);
    return audio::AudioSource{ctx.entityMap.at(id), std::move(paths), properties};
}

void SerializeCollider(std::ostream& stream, const physics::Collider& out, const SerializationContext& ctx, void*)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    const auto colliderType = out.GetType();
    serialize::Serialize(stream, colliderType);
    serialize::Serialize(stream, out.IsTrigger());
    switch (colliderType)
    {
        case physics::ColliderType::Box:
        {
            serialize::Serialize(stream, physics::ToBoxProperties(out.GetInfo()));
            break;
        }
        case physics::ColliderType::Sphere:
        {
            serialize::Serialize(stream, physics::ToSphereProperties(out.GetInfo()));
            break;
        }
        case physics::ColliderType::Capsule:
        {
            serialize::Serialize(stream, physics::ToCapsuleProperties(out.GetInfo()));
            break;
        }
        case physics::ColliderType::Hull:
        {
            serialize::Serialize(stream, physics::HullProperties{out.GetAssetPath()});
            break;
        }
    }
}

auto DeserializeCollider(std::istream& stream, const DeserializationContext& ctx, void*) -> physics::Collider
{
    auto id = uint32_t{};
    auto colliderType = physics::ColliderType{};
    auto isTrigger = false;
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, colliderType);
    serialize::Deserialize(stream, isTrigger);
    const auto entity = ctx.entityMap.at(id);
    switch (colliderType)
    {
        case physics::ColliderType::Box:
        {
            auto properties = physics::BoxProperties{};
            serialize::Deserialize(stream, properties);
            return physics::Collider{entity, properties, isTrigger};
        }
        case physics::ColliderType::Sphere:
        {
            auto properties = physics::SphereProperties{};
            serialize::Deserialize(stream, properties);
            return physics::Collider{entity, properties, isTrigger};
        }
        case physics::ColliderType::Capsule:
        {
            auto properties = physics::CapsuleProperties{};
            serialize::Deserialize(stream, properties);
            return physics::Collider{entity, properties, isTrigger};
        }
        case physics::ColliderType::Hull:
        {
            auto properties = physics::HullProperties{};
            serialize::Deserialize(stream, properties);
            return physics::Collider{entity, std::move(properties), isTrigger};
        }
        default:
        {
            throw NcError("Unexpected collider type");
        }
    }
}

void SerializeConcaveCollider(std::ostream& stream, const physics::ConcaveCollider& out, const SerializationContext& ctx, void*)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.GetAssetPath());
}

auto DeserializeConcaveCollider(std::istream& stream, const DeserializationContext& ctx, void*) -> physics::ConcaveCollider
{
    auto id = uint32_t{};
    auto geometry = std::string{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, geometry);
    return physics::ConcaveCollider{ctx.entityMap.at(id), std::move(geometry)};
}

void SerializeMeshRenderer(std::ostream& stream, const graphics::MeshRenderer& out, const SerializationContext& ctx, void*)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.GetMeshPath());
    serialize::Serialize(stream, out.GetMaterial());
    serialize::Serialize(stream, out.GetTechniqueType());
}

auto DeserializeMeshRenderer(std::istream& stream, const DeserializationContext& ctx, void*) -> graphics::MeshRenderer
{
    auto id = uint32_t{};
    auto mesh = std::string{};
    auto material = graphics::PbrMaterial{};
    auto technique = graphics::TechniqueType{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, mesh);
    serialize::Deserialize(stream, material);
    serialize::Deserialize(stream, technique);
    return graphics::MeshRenderer{ctx.entityMap.at(id), std::move(mesh), std::move(material), technique};
}

void SerializeParticleEmitter(std::ostream& stream, const graphics::ParticleEmitter& out, const SerializationContext& ctx, void*)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.GetInfo());
}

auto DeserializeParticleEmitter(std::istream& stream, const DeserializationContext& ctx, void*) -> graphics::ParticleEmitter
{
    auto id = uint32_t{};
    auto particleInfo = graphics::ParticleInfo{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, particleInfo);
    return graphics::ParticleEmitter{ctx.entityMap.at(id), particleInfo};
}

void SerializePhysicsBody(std::ostream& stream, const physics::PhysicsBody& out, const SerializationContext& ctx, void*)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.GetProperties());
    serialize::Serialize(stream, out.GetLinearFreedom());
    serialize::Serialize(stream, out.GetAngularFreedom());
}

auto DeserializePhysicsBody(std::istream& stream, const DeserializationContext& ctx, void*) -> physics::PhysicsBody
{
    auto id = uint32_t{};
    auto properties = physics::PhysicsProperties{};
    auto linearFreedom = Vector3{};
    auto angularFreedom = Vector3{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, properties);
    serialize::Deserialize(stream, linearFreedom);
    serialize::Deserialize(stream, angularFreedom);
    return physics::PhysicsBody{ctx.entityMap.at(id), properties, linearFreedom, angularFreedom};
}

void SerializePointLight(std::ostream& stream, const graphics::PointLight& out, const SerializationContext& ctx, void*)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.ambientColor);
    serialize::Serialize(stream, out.diffuseColor);
    serialize::Serialize(stream, out.diffuseIntensity);
}

auto DeserializePointLight(std::istream& stream, const DeserializationContext& ctx, void*) -> graphics::PointLight
{
    auto id = uint32_t{};
    auto ambient = Vector3{};
    auto diffuseColor = Vector3{};
    auto diffuseIntensity = 0.0f;
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, ambient);
    serialize::Deserialize(stream, diffuseColor);
    serialize::Deserialize(stream, diffuseIntensity);
    return graphics::PointLight{ctx.entityMap.at(id), ambient, diffuseColor, diffuseIntensity};
}

void SerializeToonRenderer(std::ostream& stream, const graphics::ToonRenderer& out, const SerializationContext& ctx, void*)
{
    nc::serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    nc::serialize::Serialize(stream, out.GetMeshPath());
    nc::serialize::Serialize(stream, out.GetMaterial());
}

auto DeserializeToonRenderer(std::istream& stream, const DeserializationContext& ctx, void*) -> graphics::ToonRenderer
{
    auto id = uint32_t{};
    auto mesh = std::string{};
    auto material = graphics::ToonMaterial{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, mesh);
    serialize::Deserialize(stream, material);
    return graphics::ToonRenderer{ctx.entityMap.at(id), std::move(mesh), std::move(material)};
}

} // namespace nc
