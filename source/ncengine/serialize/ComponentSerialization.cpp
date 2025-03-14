#include "ComponentSerialization.h"
#include "ncengine/asset/Assets.h"
#include "ncengine/asset/AssetViews.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/graphics/Light.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/serialize/SceneSerialization.h"
#include "physics/DeferredPhysicsCreateState.h"

#include "ncutility/BinarySerialization.h"

namespace nc
{
namespace asset
{
void Serialize(std::ostream& stream, const TextureView& in)
{
    serialize::Serialize(stream, in.id);
}

void Deserialize(std::istream& stream, TextureView& out)
{
    auto textureId = AssetId{};
    serialize::Deserialize(stream, textureId);
    out = asset::AcquireTextureAsset(textureId);
}

void Serialize(std::ostream& stream, const AudioClipView& in)
{
    serialize::Serialize(stream, in.id);
}

void Deserialize(std::istream& stream, AudioClipView& out)
{
    auto clipId = AssetId{};
    serialize::Deserialize(stream, clipId);
    out = asset::AcquireAudioClipAsset(clipId);
}
} // namespace asset

void SerializeMaterialDesc(std::ostream& stream, const MaterialInstance& out)
{
    const auto& properties = out.GetProperties();
    serialize::Serialize(stream, std::string{out.GetName()}); // don't want to serialize as string_view!
    serialize::Serialize(stream, out.GetPasses());
    serialize::Serialize(stream, properties.gradientStart); // serialize properties individually so we hit the special handling for textures
    serialize::Serialize(stream, properties.diffuseTex);
    serialize::Serialize(stream, properties.gradientEnd);
    serialize::Serialize(stream, properties.normalTex);
    serialize::Serialize(stream, properties.hatchTex);
    serialize::Serialize(stream, properties.normalIntensity);
    serialize::Serialize(stream, properties.hatchTiling);
    serialize::Serialize(stream, properties.gradientAmount);
    serialize::Serialize(stream, properties.reflectivity);
    serialize::Serialize(stream, properties.useTextureNormals);
    serialize::Serialize(stream, properties.useFlatShading);
}

auto DeserializeMaterialDesc(std::istream& stream) -> MaterialDesc
{
    auto out = MaterialDesc{};
    serialize::Deserialize(stream, out.name);
    serialize::Deserialize(stream, out.passes);
    serialize::Deserialize(stream, out.properties.gradientStart);
    serialize::Deserialize(stream, out.properties.diffuseTex);
    serialize::Deserialize(stream, out.properties.gradientEnd);
    serialize::Deserialize(stream, out.properties.normalTex);
    serialize::Deserialize(stream, out.properties.hatchTex);
    serialize::Deserialize(stream, out.properties.normalIntensity);
    serialize::Deserialize(stream, out.properties.hatchTiling);
    serialize::Deserialize(stream, out.properties.gradientAmount);
    serialize::Deserialize(stream, out.properties.reflectivity);
    serialize::Deserialize(stream, out.properties.useTextureNormals);
    serialize::Deserialize(stream, out.properties.useFlatShading);
    return out;
}

void SerializeAudioSource(std::ostream& stream, const AudioSource& out, const SerializationContext& ctx, const std::any&)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.ParentEntity()));
    serialize::Serialize(stream, out.GetClips());
    serialize::Serialize(stream, out.GetProperties());
}

auto DeserializeAudioSource(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> AudioSource
{
    auto id = uint32_t{};
    auto clips = std::vector<asset::AudioClipView>{};
    auto properties = AudioSourceProperties{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, clips);
    serialize::Deserialize(stream, properties);
    return AudioSource{ctx.entityMap.at(id), std::move(clips), properties};
}

void SerializeDirectionalLight(std::ostream& stream, const DirectionalLight& out, const SerializationContext&, const std::any&)
{
    serialize::Serialize(stream, out);
}

auto DeserializeDirectionalLight(std::istream& stream, const DeserializationContext&, const std::any&) -> DirectionalLight
{
    auto out = DirectionalLight{};
    serialize::Deserialize(stream, out);
    return out;
}

void SerializeSkinnedMesh(std::ostream& stream, const SkinnedMesh& out, const SerializationContext& ctx, const std::any&)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.GetEntity()));
    serialize::Serialize(stream, out.GetMeshId());
    SerializeMaterialDesc(stream, out.GetMaterial());
    serialize::Serialize(stream, out.GetAnimationController().GetAnimation(RootAnimationState));
}

auto DeserializeSkinnedMesh(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> SkinnedMesh
{
    auto entityId = uint32_t{};
    auto meshId = asset::AssetId{};
    auto animId = asset::AssetId{};
    serialize::Deserialize(stream, entityId);
    serialize::Deserialize(stream, meshId);
    auto materialDesc = DeserializeMaterialDesc(stream);
    serialize::Deserialize(stream, animId);

    return SkinnedMesh{
        ctx.entityMap.at(entityId),
        asset::AcquireMeshAsset(meshId),
        materialDesc,
        animId
    };
}

void SerializeStaticMesh(std::ostream& stream, const StaticMesh& out, const SerializationContext& ctx, const std::any&)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.GetEntity()));
    serialize::Serialize(stream, out.GetMeshId());
    SerializeMaterialDesc(stream, out.GetMaterial());
}

auto DeserializeStaticMesh(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> StaticMesh
{
    auto entityId = uint32_t{};
    auto meshId = asset::AssetId{};
    serialize::Deserialize(stream, entityId);
    serialize::Deserialize(stream, meshId);
    auto materialDesc = DeserializeMaterialDesc(stream);

    return StaticMesh{
        ctx.entityMap.at(entityId),
        asset::AcquireMeshAsset(meshId),
        materialDesc
    };
}

void SerializeParticleEmitter(std::ostream& stream, const ParticleEmitter& out, const SerializationContext& ctx, const std::any&)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.GetEntity()));
    serialize::Serialize(stream, out.GetTexture());
    serialize::Serialize(stream, out.GetInfo());
}

auto DeserializeParticleEmitter(std::istream& stream, const DeserializationContext& ctx, const std::any&) -> ParticleEmitter
{
    auto id = uint32_t{};
    auto texture = asset::TextureView{};
    auto particleInfo = ParticleInfo{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, texture);
    serialize::Deserialize(stream, particleInfo);
    return ParticleEmitter{ctx.entityMap.at(id), texture, particleInfo};
}

void SerializePointLight(std::ostream& stream, const PointLight& out, const SerializationContext&, const std::any&)
{
    serialize::Serialize(stream, out);
}

auto DeserializePointLight(std::istream& stream, const DeserializationContext&, const std::any&) -> PointLight
{
    auto out = PointLight{};
    serialize::Deserialize(stream, out);
    return out;
}

void SerializeSpotLight(std::ostream& stream, const SpotLight& out, const SerializationContext&, const std::any&)
{
    serialize::Serialize(stream, out);
}

auto DeserializeSpotLight(std::istream& stream, const DeserializationContext&, const std::any&) -> SpotLight
{
    auto out = SpotLight{};
    serialize::Deserialize(stream, out);
    return out;
}

void SerializeRigidBody(std::ostream& stream, const RigidBody& out, const SerializationContext& ctx, const std::any&)
{
    serialize::Serialize(stream, ctx.entityMap.at(out.GetEntity()));

    const auto& shape = out.GetShape();
    const auto shapeType = shape.GetType();
    serialize::Serialize(stream, shapeType);
    switch (shapeType)
    {
        case ShapeType::Box:
        case ShapeType::Sphere:
        case ShapeType::Capsule:
            serialize::Serialize(stream, shape.GetLocalScale());
            break;
        case ShapeType::ConvexHull:
        case ShapeType::Mesh:
            serialize::Serialize(stream, shape.GetAssetId());
            serialize::Serialize(stream, shape.GetLocalScale());
            break;
        default:
            throw NcError{fmt::format("Unknown ShapeType '{}'", std::to_underlying(shapeType))};
    }

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

auto DeserializeConstraintInfo(std::istream& stream) -> ConstraintInfo
{
    auto type = ConstraintType{};
    serialize::Deserialize(stream, type);
    switch (type)
    {
        case ConstraintType::FixedConstraint:
        {
            auto createInfo = FixedConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return ConstraintInfo{createInfo};
        }
        case ConstraintType::PointConstraint:
        {
            auto createInfo = PointConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return ConstraintInfo{createInfo};
        }
        case ConstraintType::DistanceConstraint:
        {
            auto createInfo = DistanceConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return ConstraintInfo{createInfo};
        }
        case ConstraintType::HingeConstraint:
        {
            auto createInfo = HingeConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return ConstraintInfo{createInfo};
        }
        case ConstraintType::SliderConstraint:
        {
            auto createInfo = SliderConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return ConstraintInfo{createInfo};
        }
        case ConstraintType::SwingTwistConstraint:
        {
            auto createInfo = SwingTwistConstraintInfo{};
            serialize::Deserialize(stream, createInfo);
            return ConstraintInfo{createInfo};
        }
        default:
        {
            throw NcError{fmt::format("Deserialized Unknown ConstraintType: '{}'", std::to_underlying(type))};
        }
    }
}

auto DeserializeRigidBody(std::istream& stream, const DeserializationContext& ctx, const std::any& userData) -> RigidBody
{
    auto id = uint32_t{};
    auto shapeType = ShapeType{};
    auto shapeAsset = asset::AssetId{};
    auto shapeScale = Vector3{};
    auto info = RigidBodyInfo{};
    auto constraintCount = size_t{};
    serialize::Deserialize(stream, id);
    serialize::Deserialize(stream, shapeType);
    switch (shapeType)
    {
        case ShapeType::Box:
        case ShapeType::Sphere:
        case ShapeType::Capsule:
            serialize::Deserialize(stream, shapeScale);
            break;
        case ShapeType::ConvexHull:
        case ShapeType::Mesh:
            serialize::Deserialize(stream, shapeAsset);
            serialize::Deserialize(stream, shapeScale);
            break;
        default:
            throw NcError{fmt::format("Deserialized Unknown ShapeType: '{}'", std::to_underlying(shapeType))};
    }

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
            case ShapeType::Box:        return Shape::MakeBox(shapeScale);
            case ShapeType::Sphere:     return Shape::MakeSphere(shapeScale.x * 0.5f);
            case ShapeType::Capsule:    return Shape::MakeCapsule(shapeScale.y * 2.0f, shapeScale.x * 0.5f);
            case ShapeType::ConvexHull: return Shape::MakeConvexHull(shapeAsset, shapeScale);
            case ShapeType::Mesh:       return Shape::MakeMesh(shapeAsset, shapeScale);
            default:
                throw NcError{fmt::format("Deserialized Unknown ShapeType: '{}'", std::to_underlying(shapeType))};
        }
    }();

    return RigidBody{entity, shape, info};
}
} // namespace nc
