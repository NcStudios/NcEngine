#include "gtest/gtest.h"
#include "serialize/ComponentSerialization.h"
#include "../AssetServiceStub.h"

#include "ncengine/audio/AudioSource.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SpotLight.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/serialize/SceneSerialization.h"
#include "graphics/system/ParticleEmitterSystem.h"
#include "physics/DeferredPhysicsCreateState.h"

#include "ncutility/ScopeExit.h"

#include <sstream>
#include <unordered_map>

DEFINE_ASSET_SERVICE_STUB(concaveColliderAssetManager, nc::asset::AssetType::ConcaveCollider, nc::asset::ConcaveColliderView, std::string);
DEFINE_ASSET_SERVICE_STUB(hullColliderAssetManager, nc::asset::AssetType::HullCollider, nc::asset::ConvexHullView, std::string);
DEFINE_ASSET_SERVICE_STUB(meshAssetManager, nc::asset::AssetType::Mesh, nc::asset::MeshView, std::string);
DEFINE_ASSET_SERVICE_STUB(textureAssetManager, nc::asset::AssetType::Texture, nc::asset::TextureView, std::string);

namespace nc
{
auto asset::AcquireAudioClipAsset(const std::string&) -> asset::AudioClipView
{
    static auto view = AudioClipView{};
    return view;
}

namespace graphics
{
void ParticleEmitterSystem::Emit(Entity, size_t) {}
void ParticleEmitterSystem::UpdateInfo(graphics::ParticleEmitter&) {}
} // namespace graphics


auto g_mockConstraints = std::unordered_map<nc::Entity::index_type, std::vector<nc::Constraint>>{};

auto RigidBody::AddConstraint(const ConstraintInfo& createInfo, const RigidBody& otherBody) -> Constraint&
{
    const auto key = m_self.Index();
    if (!g_mockConstraints.contains(key))
    {
        g_mockConstraints.emplace(key, std::vector<Constraint>{});
    }

    return g_mockConstraints.at(key).emplace_back(createInfo, otherBody.GetEntity(), 0u);
}

auto RigidBody::AddConstraint(const ConstraintInfo& createInfo) -> Constraint&
{
    return g_mockConstraints.at(m_self.Index()).emplace_back(createInfo, Entity::Null(), 0u);
}

auto RigidBody::GetConstraints() const -> std::span<const Constraint>
{
    return g_mockConstraints.contains(m_self.Index())
        ? g_mockConstraints.at(m_self.Index())
        : std::span<const Constraint>{};
}
} // namespace nc

auto g_registry = nc::ecs::ComponentRegistry{10ull};
auto g_ecs = nc::ecs::Ecs{g_registry};
constexpr auto g_entity = nc::Entity{42u, nc::Entity::layer_type{}, nc::Entity::Flags::None};
constexpr auto g_staticEntity = nc::Entity{42u, nc::Entity::layer_type{}, nc::Entity::Flags::Static};
auto g_entityToFragmentIdMap = nc::EntityToFragmentIdMap
{
    {g_entity, 0u},
    {g_staticEntity, 1u}
};

auto g_fragmentIdToEntityMap = nc::FragmentIdToEntityMap
{
    {0u, g_entity},
    {1u, g_staticEntity}
};

const auto g_serializationContext = nc::SerializationContext
{
    .entityMap = g_entityToFragmentIdMap,
    .ecs = g_ecs
};

auto g_deserializationContext = nc::DeserializationContext
{
    .entityMap = g_fragmentIdToEntityMap,
    .ecs = g_ecs
};

TEST(ComponentSerializationTests, RoundTrip_audioSource_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expectedClips = std::vector<std::string>{"sound1.nca", "sound2.nca"};
    const auto expectedFlags = nc::audio::AudioSourceFlags::Spatial;
    const auto expectedProperties = nc::audio::AudioSourceProperties{.flags = expectedFlags};
    const auto expected = nc::audio::AudioSource{g_entity, expectedClips, expectedProperties};
    nc::SerializeAudioSource(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeAudioSource(stream, g_deserializationContext, nullptr);
    EXPECT_TRUE(std::ranges::equal(expectedClips, actual.GetAssetPaths()));
    const auto& actualProperties = actual.GetProperties();
    EXPECT_EQ(expectedProperties.gain, actualProperties.gain);
    EXPECT_EQ(expectedProperties.innerRadius, actualProperties.innerRadius);
    EXPECT_EQ(expectedProperties.outerRadius, actualProperties.outerRadius);
    EXPECT_EQ(expectedFlags, actualProperties.flags);
}

TEST(ComponentSerializationTests, RoundTrip_meshRenderer_preservesValues)
{
    auto stream = std::stringstream{};
    auto expectedMaterial = nc::graphics::PbrMaterial{"base", "normal", "rough", "metal"};
    const auto expected = nc::graphics::MeshRenderer{g_staticEntity, "mesh.nca", expectedMaterial};
    nc::SerializeMeshRenderer(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeMeshRenderer(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.GetMeshPath(), actual.GetMeshPath());
    EXPECT_EQ(expected.GetTechniqueType(), actual.GetTechniqueType());
    const auto& actualMaterial = actual.GetMaterial();
    EXPECT_EQ(expectedMaterial.baseColor, actualMaterial.baseColor);
    EXPECT_EQ(expectedMaterial.normal, actualMaterial.normal);
    EXPECT_EQ(expectedMaterial.roughness, actualMaterial.roughness);
    EXPECT_EQ(expectedMaterial.metallic, actualMaterial.metallic);
}

TEST(ComponentSerializationTests, RoundTrip_particleEmitter_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expectedInfo = nc::graphics::ParticleInfo{};
    const auto expected = nc::graphics::ParticleEmitter{g_staticEntity, expectedInfo};
    nc::SerializeParticleEmitter(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeParticleEmitter(stream, g_deserializationContext, nullptr);
    const auto& actualInfo = actual.GetInfo();
    EXPECT_EQ(expectedInfo.emission.maxParticleCount, actualInfo.emission.maxParticleCount);
    EXPECT_EQ(expectedInfo.emission.initialEmissionCount, actualInfo.emission.initialEmissionCount);
    EXPECT_EQ(expectedInfo.emission.periodicEmissionCount, actualInfo.emission.periodicEmissionCount);
    EXPECT_EQ(expectedInfo.emission.periodicEmissionFrequency, actualInfo.emission.periodicEmissionFrequency);
    EXPECT_EQ(expectedInfo.init.lifetime, actualInfo.init.lifetime);
    EXPECT_EQ(expectedInfo.init.positionMin, actualInfo.init.positionMin);
    EXPECT_EQ(expectedInfo.init.positionMax, actualInfo.init.positionMax);
    EXPECT_EQ(expectedInfo.init.rotationMin, actualInfo.init.rotationMin);
    EXPECT_EQ(expectedInfo.init.rotationMax, actualInfo.init.rotationMax);
    EXPECT_EQ(expectedInfo.init.scaleMin, actualInfo.init.scaleMin);
    EXPECT_EQ(expectedInfo.init.scaleMax, actualInfo.init.scaleMax);
    EXPECT_EQ(expectedInfo.init.particleTexturePath, actualInfo.init.particleTexturePath);
    EXPECT_EQ(expectedInfo.kinematic.velocityMin, actualInfo.kinematic.velocityMin);
    EXPECT_EQ(expectedInfo.kinematic.velocityMax, actualInfo.kinematic.velocityMax);
    EXPECT_EQ(expectedInfo.kinematic.velocityOverTimeFactor, actualInfo.kinematic.velocityOverTimeFactor);
    EXPECT_EQ(expectedInfo.kinematic.rotationMin, actualInfo.kinematic.rotationMin);
    EXPECT_EQ(expectedInfo.kinematic.rotationMax, actualInfo.kinematic.rotationMax);
    EXPECT_EQ(expectedInfo.kinematic.rotationOverTimeFactor, actualInfo.kinematic.rotationOverTimeFactor);
    EXPECT_EQ(expectedInfo.kinematic.scaleOverTimeFactor, actualInfo.kinematic.scaleOverTimeFactor);
}

TEST(ComponentSerializationTests, RoundTrip_pointLight_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expected = nc::graphics::PointLight{nc::Vector3::Splat(2.0f), nc::Vector3::Splat(3.0f), 42.0f};
    nc::SerializePointLight(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializePointLight(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.ambientColor, actual.ambientColor);
    EXPECT_EQ(expected.diffuseColor, actual.diffuseColor);
    EXPECT_EQ(expected.radius, actual.radius);
}

TEST(ComponentSerializationTests, RoundTrip_spotLight_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expected = nc::graphics::SpotLight{nc::Vector3::Splat(1.0f), 1.0f, 1.1f, 25.0f};
    nc::SerializeSpotLight(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeSpotLight(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.color, actual.color);
    EXPECT_EQ(expected.innerAngle, actual.innerAngle);
    EXPECT_EQ(expected.outerAngle, actual.outerAngle);
    EXPECT_EQ(expected.radius, actual.radius);
}

TEST(ComponentSerializationTests, RoundTrip_toonRenderer_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expectedMaterial = nc::graphics::ToonMaterial{"base", 2, "hatch", 2};
    const auto expected = nc::graphics::ToonRenderer{g_staticEntity, "mesh.nca", expectedMaterial};
    nc::SerializeToonRenderer(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeToonRenderer(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.GetMeshPath(), actual.GetMeshPath());
    const auto& actualMaterial = actual.GetMaterial();
    EXPECT_EQ(expectedMaterial.baseColor, actualMaterial.baseColor);
    EXPECT_EQ(expectedMaterial.outlineWidth, actualMaterial.outlineWidth);
    EXPECT_EQ(expectedMaterial.hatching, actualMaterial.hatching);
    EXPECT_EQ(expectedMaterial.hatchingTiling, actualMaterial.hatchingTiling);
}

TEST(ComponentSerializationTests, RoundTrip_rigidBody_preservesValues)
{
    auto stream = std::stringstream{};
    auto deferredState = nc::physics::DeferredPhysicsCreateState{};
    auto userData = std::any{&deferredState};
    const auto expected = nc::RigidBody{
        g_entity,
        nc::Shape::MakeBox(
            nc::Vector3{2.0f, 3.0f, 4.0f},
            nc::Vector3::Splat(5.0f)
        ),
        nc::RigidBodyInfo{
            .mass = 50.0f,
            .friction = 1.0f,
            .restitution = 0.1f,
            .linearDamping = 0.8f,
            .angularDamping = 0.4f,
            .gravityMultiplier = 0.5f,
            .type = nc::BodyType::Dynamic,
            .freedom = nc::DegreeOfFreedom::Translation,
            .flags = nc::RigidBodyFlags::Trigger
        }
    };

    nc::SerializeRigidBody(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeRigidBody(stream, g_deserializationContext, userData);

    EXPECT_EQ(expected.GetEntity(), actual.GetEntity());

    const auto& expectedShape = expected.GetShape();
    const auto& actualShape = actual.GetShape();
    EXPECT_EQ(expectedShape.GetType(), actualShape.GetType());
    EXPECT_EQ(expectedShape.GetLocalPosition(), actualShape.GetLocalPosition());
    EXPECT_EQ(expectedShape.GetLocalScale(), actualShape.GetLocalScale());

    EXPECT_FLOAT_EQ(expected.GetMass(), actual.GetMass());
    EXPECT_FLOAT_EQ(expected.GetFriction(), actual.GetFriction());
    EXPECT_FLOAT_EQ(expected.GetRestitution(), actual.GetRestitution());
    EXPECT_FLOAT_EQ(expected.GetLinearDamping(), actual.GetLinearDamping());
    EXPECT_FLOAT_EQ(expected.GetAngularDamping(), actual.GetAngularDamping());
    EXPECT_FLOAT_EQ(expected.GetGravityMultiplier(), actual.GetGravityMultiplier());
    EXPECT_EQ(expected.GetBodyType(), actual.GetBodyType());
    EXPECT_EQ(expected.GetDegreesOfFreedom(), actual.GetDegreesOfFreedom());
    EXPECT_EQ(expected.GetInfo().flags, actual.GetInfo().flags);
}

TEST(ComponentSerializationTests, RoundTrip_rigidBody_box_preservesValues)
{
    auto stream = std::stringstream{};
    auto deferredState = nc::physics::DeferredPhysicsCreateState{};
    auto userData = std::any{&deferredState};
    const auto expectedShape = nc::Shape::MakeBox(
        nc::Vector3{2.0f, 3.0f, 4.0f},
        nc::Vector3::Splat(5.0f)
    );

    const auto expected = nc::RigidBody{g_entity, expectedShape};
    nc::SerializeRigidBody(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeRigidBody(stream, g_deserializationContext, userData);

    const auto& actualShape = actual.GetShape();
    EXPECT_EQ(expectedShape.GetType(), actualShape.GetType());
    EXPECT_EQ(expectedShape.GetLocalPosition(), actualShape.GetLocalPosition());
    EXPECT_EQ(expectedShape.GetLocalScale(), actualShape.GetLocalScale());
}

TEST(ComponentSerializationTests, RoundTrip_rigidBody_sphere_preservesValues)
{
    auto stream = std::stringstream{};
    auto deferredState = nc::physics::DeferredPhysicsCreateState{};
    auto userData = std::any{&deferredState};
    const auto expectedShape = nc::Shape::MakeSphere(
        7.0f,
        nc::Vector3::Splat(5.0f)
    );

    const auto expected = nc::RigidBody{g_entity, expectedShape};
    nc::SerializeRigidBody(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeRigidBody(stream, g_deserializationContext, userData);

    const auto& actualShape = actual.GetShape();
    EXPECT_EQ(expectedShape.GetType(), actualShape.GetType());
    EXPECT_EQ(expectedShape.GetLocalPosition(), actualShape.GetLocalPosition());
    EXPECT_EQ(expectedShape.GetLocalScale(), actualShape.GetLocalScale());
}

TEST(ComponentSerializationTests, RoundTrip_rigidBody_capsule_preservesValues)
{
    auto stream = std::stringstream{};
    auto deferredState = nc::physics::DeferredPhysicsCreateState{};
    auto userData = std::any{&deferredState};
    const auto expectedShape = nc::Shape::MakeCapsule(
        10.0f,
        5.0f,
        nc::Vector3::Splat(5.0f)
    );

    const auto expected = nc::RigidBody{g_entity, expectedShape};
    nc::SerializeRigidBody(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeRigidBody(stream, g_deserializationContext, userData);

    const auto& actualShape = actual.GetShape();
    EXPECT_EQ(expectedShape.GetType(), actualShape.GetType());
    EXPECT_EQ(expectedShape.GetLocalPosition(), actualShape.GetLocalPosition());
    EXPECT_EQ(expectedShape.GetLocalScale(), actualShape.GetLocalScale());
}

TEST(ComponentSerializationTests, RoundTrip_constraints_queuesToUserData)
{
    constexpr auto entity1 = nc::Entity{0u, 0, 0};
    constexpr auto entity2 = nc::Entity{1u, 0, 0};
    constexpr auto entity3 = nc::Entity{2u, 0, 0};
    auto entityToFragmentIdMap = nc::EntityToFragmentIdMap{ {entity1, 0u}, {entity2, 1u}, {entity3, 2u} };
    auto fragmentIdToEntityMap = nc::FragmentIdToEntityMap{ {0u, entity1}, {1u, entity2}, {2u, entity3} };
    const auto serializationContext = nc::SerializationContext{entityToFragmentIdMap, g_ecs};
    const auto deserializationContext = nc::DeserializationContext{fragmentIdToEntityMap, g_ecs};
    const auto shape = nc::Shape::MakeBox();
    auto body1 = nc::RigidBody{entity1, shape};
    auto body2 = nc::RigidBody{entity2, shape};
    auto body3 = nc::RigidBody{entity3, shape};

    body1.AddConstraint(nc::FixedConstraintInfo{}, body2);
    body1.AddConstraint(nc::PointConstraintInfo{}, body3);
    body2.AddConstraint(nc::DistanceConstraintInfo{}, body1);
    body2.AddConstraint(nc::HingeConstraintInfo{}, body3);
    body3.AddConstraint(nc::SliderConstraintInfo{}, body1);
    body3.AddConstraint(nc::SwingTwistConstraintInfo{}, body2);

    ASSERT_EQ(2u, std::as_const(body1).GetConstraints().size());
    ASSERT_EQ(2u, std::as_const(body2).GetConstraints().size());
    ASSERT_EQ(2u, std::as_const(body3).GetConstraints().size());
    const auto& expectedConstraint1 = std::as_const(body1).GetConstraints()[0];
    const auto& expectedConstraint2 = std::as_const(body1).GetConstraints()[1];
    const auto& expectedConstraint3 = std::as_const(body2).GetConstraints()[0];
    const auto& expectedConstraint4 = std::as_const(body2).GetConstraints()[1];
    const auto& expectedConstraint5 = std::as_const(body3).GetConstraints()[0];
    const auto& expectedConstraint6 = std::as_const(body3).GetConstraints()[1];

    auto stream = std::stringstream{};
    auto deferredState = nc::physics::DeferredPhysicsCreateState{};
    auto userData = std::any{&deferredState};
    nc::SerializeRigidBody(stream, entity1, serializationContext, userData);
    nc::SerializeRigidBody(stream, entity2, serializationContext, userData);
    nc::SerializeRigidBody(stream, entity3, serializationContext, userData);
    nc::DeserializeRigidBody(stream, deserializationContext, userData);
    nc::DeserializeRigidBody(stream, deserializationContext, userData);
    nc::DeserializeRigidBody(stream, deserializationContext, userData);

    // deserialized bodies won't have constraints, they'll be in user data
    const auto& actualConstraints = deferredState.constraints;
    ASSERT_EQ(6u, actualConstraints.size());

    const auto& actualConstraint1 = actualConstraints.at(0);
    const auto& actualConstraint2 = actualConstraints.at(1);
    const auto& actualConstraint3 = actualConstraints.at(2);
    const auto& actualConstraint4 = actualConstraints.at(3);
    const auto& actualConstraint5 = actualConstraints.at(4);
    const auto& actualConstraint6 = actualConstraints.at(5);

    EXPECT_EQ(entity1, actualConstraint1.owner);
    EXPECT_EQ(entity1, actualConstraint2.owner);
    EXPECT_EQ(entity2, actualConstraint3.owner);
    EXPECT_EQ(entity2, actualConstraint4.owner);
    EXPECT_EQ(entity3, actualConstraint5.owner);
    EXPECT_EQ(entity3, actualConstraint6.owner);
    EXPECT_EQ(expectedConstraint1.GetConstraintTarget(), actualConstraint1.target);
    EXPECT_EQ(expectedConstraint2.GetConstraintTarget(), actualConstraint2.target);
    EXPECT_EQ(expectedConstraint3.GetConstraintTarget(), actualConstraint3.target);
    EXPECT_EQ(expectedConstraint4.GetConstraintTarget(), actualConstraint4.target);
    EXPECT_EQ(expectedConstraint5.GetConstraintTarget(), actualConstraint5.target);
    EXPECT_EQ(expectedConstraint6.GetConstraintTarget(), actualConstraint6.target);

    EXPECT_NO_THROW((void)std::get<nc::FixedConstraintInfo>(actualConstraint1.info));
    EXPECT_NO_THROW((void)std::get<nc::PointConstraintInfo>(actualConstraint2.info));
    EXPECT_NO_THROW((void)std::get<nc::DistanceConstraintInfo>(actualConstraint3.info));
    EXPECT_NO_THROW((void)std::get<nc::HingeConstraintInfo>(actualConstraint4.info));
    EXPECT_NO_THROW((void)std::get<nc::SliderConstraintInfo>(actualConstraint5.info));
    EXPECT_NO_THROW((void)std::get<nc::SwingTwistConstraintInfo>(actualConstraint6.info));
}
