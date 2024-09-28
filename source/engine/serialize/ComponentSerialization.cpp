#include "ComponentSerialization.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SpotLight.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/physics/PhysicsMaterial.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/serialize/SceneSerialization.h"
#include "physics/ColliderUtility.h"
#include "physics2/DeferredPhysicsCreateState.h"

#include "ncutility/BinarySerialization.h"

namespace nc
{
void SerializeAudioSource(std::ostream& stream, const audio::AudioSource& out, const SerializationContext& ctx, const std::any&)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.GetAssetPaths());
    serialize::Serialize(stream, out.GetProperties());
}

auto DeserializeAudioSource(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> audio::AudioSource
{
    auto id = uint32_t{};
    auto paths = std::vector<std::string>{};
    auto properties = audio::AudioSourceProperties{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, paths);
    serialize::Deserialize(stream, properties);
    return audio::AudioSource{ctx.entityMap.at(id), std::move(paths), properties};
}

void SerializeCollider(std::ostream& stream, const physics::Collider& out, const SerializationContext& ctx, const std::any&)
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

auto DeserializeCollider(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> physics::Collider
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

void SerializeConcaveCollider(std::ostream& stream, const physics::ConcaveCollider& out, const SerializationContext& ctx, const std::any&)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.GetPath());
}

auto DeserializeConcaveCollider(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> physics::ConcaveCollider
{
    auto id = uint32_t{};
    auto geometry = std::string{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, geometry);
    return physics::ConcaveCollider{ctx.entityMap.at(id), std::move(geometry)};
}

void SerializeMeshRenderer(std::ostream& stream, const graphics::MeshRenderer& out, const SerializationContext& ctx, const std::any&)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.GetMeshPath());
    serialize::Serialize(stream, out.GetMaterial());
    serialize::Serialize(stream, out.GetTechniqueType());
}

auto DeserializeMeshRenderer(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::MeshRenderer
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

void SerializeOrientationClamp(std::ostream& stream, const physics::OrientationClamp& out, const SerializationContext&, const std::any&)
{
    serialize::Serialize(stream, out);
}

auto DeserializeOrientationClamp(std::istream& stream, const DeserializationContext&, const std::any&) -> physics::OrientationClamp
{
    auto out = physics::OrientationClamp{};
    serialize::Deserialize(stream, out);
    return out;
}

void SerializeParticleEmitter(std::ostream& stream, const graphics::ParticleEmitter& out, const SerializationContext& ctx, const std::any&)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.GetInfo());
}

auto DeserializeParticleEmitter(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::ParticleEmitter
{
    auto id = uint32_t{};
    auto particleInfo = graphics::ParticleInfo{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, particleInfo);
    return graphics::ParticleEmitter{ctx.entityMap.at(id), particleInfo};
}

void SerializePhysicsBody(std::ostream& stream, const physics::PhysicsBody& out, const SerializationContext& ctx, const std::any& userData)
{
    const auto registry = std::any_cast<ecs::ComponentRegistry*>(userData);
    const auto entity = registry->GetPool<physics::PhysicsBody>().GetParent(&out);
    NC_ASSERT(entity.Valid(), "Invalid parent entity for PhysicsBody");
    serialize::Serialize(stream, ctx.entityMap.at(entity));
    auto properties = out.GetProperties();
    if (properties.mass != 0.0f)
    {
        properties.mass = 1.0f / properties.mass;
    }

    serialize::Serialize(stream, properties);
}

auto DeserializePhysicsBody(std::istream& stream, const DeserializationContext& ctx, const std::any& userData) -> physics::PhysicsBody
{
    auto id = uint32_t{};
    auto properties = physics::PhysicsProperties{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, properties);
    const auto entity = ctx.entityMap.at(id);
    const auto registry = std::any_cast<ecs::ComponentRegistry*>(userData);
    return physics::PhysicsBody{registry->GetPool<Transform>().Get(entity), registry->GetPool<physics::Collider>().Get(entity), properties};
}

void SerializePhysicsMaterial(std::ostream& stream, const physics::PhysicsMaterial& out, const SerializationContext&, const std::any&)
{
    serialize::Serialize(stream, out.friction);
    serialize::Serialize(stream, out.restitution);
}

auto DeserializePhysicsMaterial(std::istream& stream, const DeserializationContext&, const std::any&) -> physics::PhysicsMaterial
{
    float friction;
    float restitution;
    serialize::Deserialize(stream, friction);
    serialize::Deserialize(stream, restitution);
    return physics::PhysicsMaterial{friction, restitution};
}

void SerializePointLight(std::ostream& stream, const graphics::PointLight& out, const SerializationContext&, const std::any&)
{
    serialize::Serialize(stream, out);
}

auto DeserializePointLight(std::istream& stream, const DeserializationContext&, const std::any&) -> graphics::PointLight
{
    auto out = graphics::PointLight{};
    serialize::Deserialize(stream, out);
    return out;
}

void SerializePositionClamp(std::ostream& stream, const physics::PositionClamp& out, const SerializationContext&, const std::any&)
{
    serialize::Serialize(stream, out);
}

auto DeserializePositionClamp(std::istream& stream, const DeserializationContext&, const std::any&) -> physics::PositionClamp
{
    auto out = physics::PositionClamp{};
    serialize::Deserialize(stream, out);
    return out;
}

void SerializeSpotLight(std::ostream& stream, const graphics::SpotLight& out, const SerializationContext&, const std::any&)
{
    serialize::Serialize(stream, out);
}

auto DeserializeSpotLight(std::istream& stream, const DeserializationContext&, const std::any&) -> graphics::SpotLight
{
    auto out = graphics::SpotLight{};
    serialize::Deserialize(stream, out);
    return out;
}

void SerializeToonRenderer(std::ostream& stream, const graphics::ToonRenderer& out, const SerializationContext& ctx, const std::any&)
{
    nc::serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    nc::serialize::Serialize(stream, out.GetMeshPath());
    nc::serialize::Serialize(stream, out.GetMaterial());
}

auto DeserializeToonRenderer(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> graphics::ToonRenderer
{
    auto id = uint32_t{};
    auto mesh = std::string{};
    auto material = graphics::ToonMaterial{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, mesh);
    serialize::Deserialize(stream, material);
    return graphics::ToonRenderer{ctx.entityMap.at(id), std::move(mesh), std::move(material)};
}

void SerializeVelocityRestriction(std::ostream& stream, const physics::VelocityRestriction& out, const SerializationContext&, const std::any&)
{
    serialize::Serialize(stream, out);
}

auto DeserializeVelocityRestriction(std::istream& stream, const DeserializationContext&, const std::any&) -> physics::VelocityRestriction
{
    auto out = physics::VelocityRestriction{};
    serialize::Deserialize(stream, out);
    return out;
}

void SerializeRigidBody(std::ostream& stream, const physics::RigidBody& out, const SerializationContext& ctx, const std::any&)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.GetEntity()));

    const auto& shape = out.GetShape();
    serialize::Serialize(stream, shape.GetType());
    serialize::Serialize(stream, shape.GetLocalScale());
    serialize::Serialize(stream, shape.GetLocalPosition());
    serialize::Serialize(stream, out.GetInfo());

    auto&& constraints = out.GetConstraints();
    serialize::Serialize(stream, constraints.size());
    for (const auto& constraint : constraints)
    {
        std::visit(
            [&](const auto& constraintInfo) {
                serialize::Serialize(stream, constraintInfo.type);
                serialize::Serialize(stream, constraintInfo);
            },
            constraint.GetInfo()
        );

        const auto target = constraint.GetConstraintTarget();
        const auto targetId = target.Valid() ? ctx.entityMap.at(target) : Entity::NullIndex;
        serialize::Serialize(stream, targetId);
    }
}

auto DeserializeConstraintInfo(std::istream& stream) -> physics::ConstraintInfo
{
    auto type = physics::ConstraintType{};
    serialize::Deserialize(stream, type);
    switch (type)
    {
        case physics::ConstraintType::FixedConstraint:
        {
            auto createInfo = physics::FixedConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return physics::ConstraintInfo{createInfo};
        }
        case physics::ConstraintType::PointConstraint:
        {
            auto createInfo = physics::PointConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return physics::ConstraintInfo{createInfo};
        }
        case physics::ConstraintType::DistanceConstraint:
        {
            auto createInfo = physics::DistanceConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return physics::ConstraintInfo{createInfo};
        }
        case physics::ConstraintType::HingeConstraint:
        {
            auto createInfo = physics::HingeConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return physics::ConstraintInfo{createInfo};
        }
        case physics::ConstraintType::SliderConstraint:
        {
            auto createInfo = physics::SliderConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return physics::ConstraintInfo{createInfo};
        }
        case physics::ConstraintType::SwingTwistConstraint:
        {
            auto createInfo = physics::SwingTwistConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return physics::ConstraintInfo{createInfo};
        }
        default:
        {
            throw NcError{fmt::format("Deserialized Unknown ConstraintType: '{}'", std::to_underlying(type))};
        }
    }
}

auto DeserializeRigidBody(std::istream& stream, const DeserializationContext& ctx, const std::any& userData) -> physics::RigidBody
{
    auto id = uint32_t{};
    auto shapeType = physics::ShapeType{};
    auto shapeScale = Vector3{};
    auto shapePosition = Vector3{};
    auto info = physics::RigidBodyInfo{};
    auto constraintCount = size_t{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, shapeType);
    serialize::Deserialize(stream, shapeScale);
    serialize::Deserialize(stream, shapePosition);
    serialize::Deserialize(stream, info);
    serialize::Deserialize(stream, constraintCount);

    auto deferredState = std::any_cast<nc::physics::DeferredPhysicsCreateState*>(userData);
    NC_ASSERT(deferredState, "RigidBody user data did not contain DeferredPhysicsCreateState");
    const auto entity = ctx.entityMap.at(id);

    for (auto i = 0u; i < constraintCount; ++i)
    {
        auto constraintInfo = DeserializeConstraintInfo(stream);
        auto targetId = uint32_t{};
        serialize::Deserialize(stream, targetId);
        const auto target = targetId == Entity::NullIndex ? Entity::Null() : ctx.entityMap.at(targetId);
        deferredState->constraints.emplace_back(entity, target, constraintInfo);
    }

    const auto shape = [&]()
    {
        using namespace nc::physics;
        switch (shapeType)
        {
            case ShapeType::Box:     return Shape::MakeBox(shapeScale, shapePosition);
            case ShapeType::Sphere:  return Shape::MakeSphere(shapeScale.x * 0.5f, shapePosition);
            case ShapeType::Capsule: return Shape::MakeCapsule(shapeScale.y * 2.0f, shapeScale.x * 0.5f, shapePosition);
            default:
                throw NcError{fmt::format("Deserialized Unknown ShapeType: '{}'", std::to_underlying(shapeType))};
        }
    }();

    return physics::RigidBody{entity, shape, info};
}
} // namespace nc
