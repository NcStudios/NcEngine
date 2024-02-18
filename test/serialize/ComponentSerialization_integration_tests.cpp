#include "gtest/gtest.h"
#include "serialize/ComponentSerialization.h"
#include "../AssetServiceStub.h"

#include "ncengine/audio/AudioSource.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/serialize/SceneSerialization.h"
#include "graphics/system/ParticleEmitterSystem.h"

#include "ncutility/ScopeExit.h"

#include <sstream>

DEFINE_ASSET_SERVICE_STUB(concaveColliderAssetManager, nc::asset::AssetType::ConcaveCollider, nc::ConcaveColliderView, std::string);
DEFINE_ASSET_SERVICE_STUB(hullColliderAssetManager, nc::asset::AssetType::HullCollider, nc::ConvexHullView, std::string);
DEFINE_ASSET_SERVICE_STUB(meshAssetManager, nc::asset::AssetType::Mesh, nc::MeshView, std::string);
DEFINE_ASSET_SERVICE_STUB(textureAssetManager, nc::asset::AssetType::Texture, nc::TextureView, std::string);

namespace nc
{
auto AcquireAudioClipAsset(const std::string&) -> AudioClipView
{
    static auto view = AudioClipView{};
    return view;
}

namespace graphics
{
void ParticleEmitterSystem::Emit(Entity, size_t) {}
void ParticleEmitterSystem::UpdateInfo(graphics::ParticleEmitter&) {}
} // namespace graphics
} // namespace nc

// We only need the old Registry here so that it sets the ptr for ActiveRegistry(), which is only used by PhysicsBody.
auto g_registry = nc::ecs::ComponentRegistry{10ull};
auto g_legacyRegistry = nc::Registry{g_registry};
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

TEST(ComponentSerializationTests, RoundTrip_collider_box_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expected = nc::physics::Collider{g_entity, nc::physics::BoxProperties{}, false};
    nc::SerializeCollider(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeCollider(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.IsTrigger(), actual.IsTrigger());
    EXPECT_EQ(expected.GetAssetPath(), actual.GetAssetPath());
    const auto& expectedInfo = expected.GetInfo();
    const auto& actualInfo = actual.GetInfo();
    EXPECT_EQ(expectedInfo.type, actualInfo.type);
    EXPECT_EQ(expectedInfo.offset, actualInfo.offset);
    EXPECT_EQ(expectedInfo.scale, actualInfo.scale);
}

TEST(ComponentSerializationTests, RoundTrip_collider_capsule_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expected = nc::physics::Collider{g_entity, nc::physics::CapsuleProperties{}, false};
    nc::SerializeCollider(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeCollider(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.IsTrigger(), actual.IsTrigger());
    EXPECT_EQ(expected.GetAssetPath(), actual.GetAssetPath());
    const auto& expectedInfo = expected.GetInfo();
    const auto& actualInfo = actual.GetInfo();
    EXPECT_EQ(expectedInfo.type, actualInfo.type);
    EXPECT_EQ(expectedInfo.offset, actualInfo.offset);
    EXPECT_EQ(expectedInfo.scale, actualInfo.scale);
}

TEST(ComponentSerializationTests, RoundTrip_collider_sphere_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expected = nc::physics::Collider{g_entity, nc::physics::SphereProperties{}, false};
    nc::SerializeCollider(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeCollider(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.IsTrigger(), actual.IsTrigger());
    EXPECT_EQ(expected.GetAssetPath(), actual.GetAssetPath());
    const auto& expectedInfo = expected.GetInfo();
    const auto& actualInfo = actual.GetInfo();
    EXPECT_EQ(expectedInfo.type, actualInfo.type);
    EXPECT_EQ(expectedInfo.offset, actualInfo.offset);
    EXPECT_EQ(expectedInfo.scale, actualInfo.scale);
}

TEST(ComponentSerializationTests, RoundTrip_collider_hull_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expected = nc::physics::Collider{g_entity, nc::physics::HullProperties{"geometry.nca"}, false};
    nc::SerializeCollider(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeCollider(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.IsTrigger(), actual.IsTrigger());
    EXPECT_EQ(expected.GetAssetPath(), actual.GetAssetPath());
    const auto& expectedInfo = expected.GetInfo();
    const auto& actualInfo = actual.GetInfo();
    EXPECT_EQ(expectedInfo.type, actualInfo.type);
    EXPECT_EQ(expectedInfo.offset, actualInfo.offset);
    EXPECT_EQ(expectedInfo.scale, actualInfo.scale);
}

TEST(ComponentSerializationTests, RoundTrip_concaveCollider_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expected = nc::physics::ConcaveCollider{g_staticEntity, "geometry.nca"};
    nc::SerializeConcaveCollider(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeConcaveCollider(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.GetPath(), actual.GetPath());
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
    const auto expected = nc::graphics::PointLight{g_staticEntity, nc::Vector3::Splat(2.0f), nc::Vector3::Splat(3.0f), 42.0f};
    nc::SerializePointLight(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializePointLight(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.GetAmbient(), actual.GetAmbient());
    EXPECT_EQ(expected.GetDiffuseColor(), actual.GetDiffuseColor());
    EXPECT_EQ(expected.GetDiffuseIntensity(), actual.GetDiffuseIntensity());
}

TEST(ComponentSerializationTests, RoundTrip_toonRenderer_preservesValues)
{
    auto stream = std::stringstream{};
    const auto expectedMaterial = nc::graphics::ToonMaterial{"base", "overlay", "hatch", 2};
    const auto expected = nc::graphics::ToonRenderer{g_staticEntity, "mesh.nca", expectedMaterial};
    nc::SerializeToonRenderer(stream, expected, g_serializationContext, nullptr);
    const auto actual = nc::DeserializeToonRenderer(stream, g_deserializationContext, nullptr);
    EXPECT_EQ(expected.GetMeshPath(), actual.GetMeshPath());
    const auto& actualMaterial = actual.GetMaterial();
    EXPECT_EQ(expectedMaterial.baseColor, actualMaterial.baseColor);
    EXPECT_EQ(expectedMaterial.overlay, actualMaterial.overlay);
    EXPECT_EQ(expectedMaterial.hatching, actualMaterial.hatching);
    EXPECT_EQ(expectedMaterial.hatchingTiling, actualMaterial.hatchingTiling);
}

TEST(ComponentSerializationTests, RoundTrip_physicsBody_preservesValues)
{
    // We actually need a Collider in the Registry for this test b/c PhysicsBody calls ActiveRegistry() :(
    // Best to keep it last, but we'll still be good citizens and clean up
    SCOPE_EXIT
    (
        g_registry.CommitPendingChanges();
        g_registry.Clear()
    );

    g_registry.RegisterType<nc::ecs::detail::FreeComponentGroup>(1);
    g_registry.RegisterType<nc::Tag>(1);
    g_registry.RegisterType<nc::Transform>(1);
    g_registry.RegisterType<nc::Hierarchy>(1);
    g_registry.RegisterType<nc::physics::Collider>(1);

    const auto entity = g_ecs.Emplace<nc::Entity>(nc::EntityInfo{});
    g_ecs.Emplace<nc::physics::Collider>(entity, nc::physics::BoxProperties{});

    auto entityToFragmentId = nc::EntityToFragmentIdMap{ {entity, 0u} };
    auto fragmentIdToEntity = nc::FragmentIdToEntityMap{ {0u, entity} };
    const auto serializeCtx = nc::SerializationContext{entityToFragmentId, g_ecs};
    auto deserializeCtx = nc::DeserializationContext{fragmentIdToEntity, g_ecs};

    auto stream = std::stringstream{};
    const auto expectedProperties = nc::physics::PhysicsProperties{};
    const auto expectedLinearFreedom = nc::Vector3::One();
    const auto expectedAngularFreedom = nc::Vector3::Up();
    const auto expected = nc::physics::PhysicsBody{entity, expectedProperties, expectedLinearFreedom, expectedAngularFreedom};
    nc::SerializePhysicsBody(stream, expected, serializeCtx, nullptr);
    const auto actual = nc::DeserializePhysicsBody(stream, deserializeCtx, nullptr);
    const auto& actualProperties = actual.GetProperties();
    EXPECT_EQ(expectedProperties.mass, actualProperties.mass);
    EXPECT_EQ(expectedProperties.drag, actualProperties.drag);
    EXPECT_EQ(expectedProperties.angularDrag, actualProperties.angularDrag);
    EXPECT_EQ(expectedProperties.restitution, actualProperties.restitution);
    EXPECT_EQ(expectedProperties.friction, actualProperties.friction);
    EXPECT_EQ(expectedProperties.useGravity, actualProperties.useGravity);
    EXPECT_EQ(expectedProperties.isKinematic, actualProperties.isKinematic);
    EXPECT_EQ(expectedLinearFreedom, actual.GetLinearFreedom());
    EXPECT_EQ(expectedAngularFreedom, actual.GetAngularFreedom());
}
