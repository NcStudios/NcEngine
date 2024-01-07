#include "gtest/gtest.h"
#include "ncengine/serialize/SceneSerialization.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Registry.h"

#include "ncutility/BinarySerialization.h"

#include <set>
#include <sstream>
#include <ranges>

namespace nc::asset
{
class NcAssetMock : public NcAsset
{
    public:
        // Stubs
        auto OnBoneUpdate() noexcept -> Signal<const BoneUpdateEventData&>& override { return m_boneSignal; }
        auto OnCubeMapUpdate() noexcept -> Signal<const CubeMapUpdateEventData&>& override { return m_cubeMapSignal; }
        auto OnMeshUpdate() noexcept -> Signal<const MeshUpdateEventData&>& override { return m_meshSignal; }
        auto OnSkeletalAnimationUpdate() noexcept -> Signal<const SkeletalAnimationUpdateEventData&>& override { return m_animSignal; }
        auto OnTextureUpdate() noexcept -> Signal<const TextureUpdateEventData&>& override { return m_textureSignal; }

        // Mocks
        void LoadAssets(const AssetMap& assets) override
        {
            m_assets.insert(std::cbegin(assets), std::cend(assets));
        }

        auto GetLoadedAssets() const noexcept -> AssetMap override
        {
            return m_assets;
        }

    private:
        AssetMap m_assets;

        Signal<const BoneUpdateEventData&> m_boneSignal;
        Signal<const CubeMapUpdateEventData&> m_cubeMapSignal;
        Signal<const MeshUpdateEventData&> m_meshSignal;
        Signal<const SkeletalAnimationUpdateEventData&> m_animSignal;
        Signal<const TextureUpdateEventData&> m_textureSignal;
};

auto BuildAssetModule(const config::AssetSettings&,
                      const config::MemorySettings&,
                      AssetMap) -> std::unique_ptr<NcAsset>
{
    return std::make_unique<NcAssetMock>();
}
} // namespace nc::asset

struct TestComponent1
{
    int value = 0;

    static void Serialize(std::ostream& stream,
                          const TestComponent1& obj,
                          const nc::SerializationContext&,
                          void*)
    {
        nc::serialize::Serialize(stream, obj.value);
    }

    static auto Deserialize(std::istream& stream,
                            const nc::DeserializationContext&,
                            void*) -> TestComponent1
    {
        auto obj = TestComponent1{};
        nc::serialize::Deserialize(stream, obj.value);
        return obj;
    }
};

struct TestComponent2
{
    std::string value = "";

    static void Serialize(std::ostream& stream,
                          const TestComponent2& obj,
                          const nc::SerializationContext&,
                          void*)
    {
        nc::serialize::Serialize(stream, obj.value);
    }

    static auto Deserialize(std::istream& stream,
                            const nc::DeserializationContext&,
                            void*) -> TestComponent2
    {
        auto obj = TestComponent2{};
        nc::serialize::Deserialize(stream, obj.value);
        return obj;
    }
};

struct UnserializableTestComponent
{
    int value = 0;
};

class SceneSerializationTests : public ::testing::Test
{
    public:
        static constexpr auto maxEntities = 10ull;
        nc::Registry legacyRegistry; // unfortunately we need this around for ActiveRegistry() usage inside Transform
        nc::ecs::ComponentRegistry& registry;
        nc::ecs::Ecs ecs;
        std::unique_ptr<nc::asset::NcAsset> assetModule;

        SceneSerializationTests()
            : legacyRegistry{maxEntities},
              registry{legacyRegistry.GetImpl()},
              ecs{registry},
              assetModule{nc::asset::BuildAssetModule(nc::config::AssetSettings{},
                                                      nc::config::MemorySettings{},
                                                      nc::asset::AssetMap{})}
        {
            registry.RegisterType<nc::ecs::detail::FreeComponentGroup>(maxEntities, nc::ComponentHandler<nc::ecs::detail::FreeComponentGroup>{});
            registry.RegisterType<nc::Tag>(maxEntities, nc::ComponentHandler<nc::Tag>{});
            registry.RegisterType<nc::Transform>(maxEntities, nc::ComponentHandler<nc::Transform>{});
            registry.RegisterType<UnserializableTestComponent>(maxEntities);
            registry.RegisterType<TestComponent1>
            (
                maxEntities,
                nc::ComponentHandler<TestComponent1>
                {
                    .serialize = TestComponent1::Serialize,
                    .deserialize = TestComponent1::Deserialize
                }
            );

            registry.RegisterType<TestComponent2>
            (
                maxEntities,
                nc::ComponentHandler<TestComponent2>
                {
                    .serialize = TestComponent2::Serialize,
                    .deserialize = TestComponent2::Deserialize
                }
            );
        }

        ~SceneSerializationTests()
        {
            legacyRegistry.Reset();
        }
};

TEST_F(SceneSerializationTests, LoadSceneFragment_onlyHasHeader_succeeds)
{
    auto stream = std::stringstream{};
    const auto header = nc::SceneFragmentHeader{nc::g_sceneFragmentMagicNumber, nc::g_currentSceneFragmentVersion};
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    EXPECT_NO_THROW(nc::LoadSceneFragment(stream, ecs, *assetModule));
}

TEST_F(SceneSerializationTests, LoadSceneFragment_badMagicNumber_throws)
{
    auto stream = std::stringstream{};
    const auto header = nc::SceneFragmentHeader{nc::g_sceneFragmentMagicNumber + 1, nc::g_currentSceneFragmentVersion};
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    EXPECT_THROW(nc::LoadSceneFragment(stream, ecs, *assetModule), nc::NcError);
}

TEST_F(SceneSerializationTests, LoadSceneFragment_badVersion_throws)
{
    auto stream = std::stringstream{};
    const auto header = nc::SceneFragmentHeader{nc::g_sceneFragmentMagicNumber, nc::g_currentSceneFragmentVersion + 1};
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    EXPECT_THROW(nc::LoadSceneFragment(stream, ecs, *assetModule), nc::NcError);
}

TEST_F(SceneSerializationTests, RoundTrip_emptyGameState_succeeds)
{
    auto stream = std::stringstream{};
    nc::SaveSceneFragment(stream, ecs, assetModule->GetLoadedAssets());
    nc::LoadSceneFragment(stream, ecs, *assetModule);
    EXPECT_TRUE(assetModule->GetLoadedAssets().empty());
    EXPECT_TRUE(ecs.GetAll<nc::Entity>().empty());
}

TEST_F(SceneSerializationTests, RoundTrip_hasLoadedAssets_correctlyRestoresAssets)
{
    const auto expectedAssets = nc::asset::AssetMap
    {
        {nc::asset::AssetType::AudioClip,         std::vector<std::string>{"ac"}},
        {nc::asset::AssetType::CubeMap,           std::vector<std::string>{"cm1", "cm2"}},
        {nc::asset::AssetType::ConcaveCollider,   std::vector<std::string>{"cc"}},
        {nc::asset::AssetType::HullCollider,      std::vector<std::string>{"hc"}},
        {nc::asset::AssetType::Mesh,              std::vector<std::string>{"m1", "m2", "m3"}},
        {nc::asset::AssetType::SkeletalAnimation, std::vector<std::string>{"sa"}},
        {nc::asset::AssetType::Texture,           std::vector<std::string>{"t"}}
    };

    assetModule->LoadAssets(expectedAssets);

    auto stream = std::stringstream{};
    nc::SaveSceneFragment(stream, ecs, assetModule->GetLoadedAssets());
    nc::LoadSceneFragment(stream, ecs, *assetModule);
    EXPECT_TRUE(ecs.GetAll<nc::Entity>().empty());
    const auto actualAssets = assetModule->GetLoadedAssets();
    ASSERT_FALSE(actualAssets.empty());
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::AudioClip),
                                   actualAssets.at(nc::asset::AssetType::AudioClip)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::CubeMap),
                                   actualAssets.at(nc::asset::AssetType::CubeMap)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::ConcaveCollider),
                                   actualAssets.at(nc::asset::AssetType::ConcaveCollider)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::HullCollider),
                                   actualAssets.at(nc::asset::AssetType::HullCollider)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::Mesh),
                                   actualAssets.at(nc::asset::AssetType::Mesh)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::SkeletalAnimation),
                                   actualAssets.at(nc::asset::AssetType::SkeletalAnimation)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::Texture),
                                   actualAssets.at(nc::asset::AssetType::Texture)));
}

TEST_F(SceneSerializationTests, RoundTrip_hasEntities_correctlyRestoresEntityValues)
{
    const auto expectedInfos = std::vector<nc::EntityInfo>
    {
        {},
        {
            .position = nc::Vector3::Up(),
            .rotation = nc::Quaternion::FromEulerAngles(1.0f, 2.0f, 3.0f),
            .scale = nc::Vector3{5.0f, 10.0f, 20.0f},
        },
        {
            .tag = "last",
            .layer = nc::Entity::layer_type{2},
            .flags = nc::Entity::Flags::Static
        },
        {.flags = nc::Entity::Flags::NoCollisionNotifications}
    };

    const auto unexpectedInfos = std::vector<nc::EntityInfo>
    {
        {.tag = "miss me", .flags = nc::Entity::Flags::Persistent},
        {.tag = "maybe next week", .flags = nc::Entity::Flags::Persistent}
    };

    auto emplaceEntity = [&ecs = ecs](const auto& info) { ecs.Emplace<nc::Entity>(info); };
    std::ranges::for_each(expectedInfos, emplaceEntity);
    std::ranges::for_each(unexpectedInfos, emplaceEntity);

    auto stream = std::stringstream{};
    const auto filter = [](nc::Entity entity) { return !entity.IsPersistent(); };
    nc::SaveSceneFragment(stream, ecs, nc::asset::AssetMap{}, filter);

    registry.CommitPendingChanges();
    registry.Clear();

    nc::LoadSceneFragment(stream, ecs, *assetModule);

    const auto entities = ecs.GetAll<nc::Entity>();
    ASSERT_EQ(expectedInfos.size(), entities.size());
    for (const auto& [expected, actualEntity] : std::views::zip(expectedInfos, entities))
    {
        EXPECT_EQ(expected.layer, actualEntity.Layer());
        EXPECT_EQ(expected.flags, actualEntity.Flags());

        const auto actualTransform = ecs.Get<nc::Transform>(actualEntity);
        ASSERT_NE(nullptr, actualTransform);
        EXPECT_EQ(expected.position, actualTransform->LocalPosition());
        EXPECT_EQ(expected.rotation, actualTransform->LocalRotation());
        EXPECT_EQ(expected.scale, actualTransform->LocalScale());

        const auto actualTag = ecs.Get<nc::Tag>(actualEntity);
        ASSERT_NE(nullptr, actualTag);
        EXPECT_EQ(expected.tag, actualTag->Value());
    }
}

TEST_F(SceneSerializationTests, RoundTrip_hasEntityHierarchy_correctlyRestoresHierarchy)
{
    {
        const auto e1 = ecs.Emplace<nc::Entity>(nc::EntityInfo{.tag = "1"});
        const auto e2 = ecs.Emplace<nc::Entity>(nc::EntityInfo{.parent = e1, .tag = "2"});
        ecs.Emplace<nc::Entity>(nc::EntityInfo{.parent = e2, .tag = "3"});
        const auto e4 = ecs.Emplace<nc::Entity>(nc::EntityInfo{.tag = "4"});
        ecs.Get<nc::Transform>(e1)->SetParent(e4); // Disrupt parenting order to ensure we're sorting correctly prior to saving
        const auto e5 = ecs.Emplace<nc::Entity>(nc::EntityInfo{ .tag = "exlude me"}); // will filter this out
        ecs.Emplace<nc::Entity>(nc::EntityInfo{ .parent = e5 }); // expect this to get filtered as well
    }

    const auto filter = [&ecs = ecs](nc::Entity entity)
    {
        static constexpr auto toExclude = std::string_view{"exlude me"};
        return ecs.Get<nc::Tag>(entity)->Value() != toExclude;
    };

    auto stream = std::stringstream{};
    nc::SaveSceneFragment(stream, ecs, nc::asset::AssetMap{}, filter);

    registry.CommitPendingChanges();
    registry.Clear();

    nc::LoadSceneFragment(stream, ecs, *assetModule);

    const auto actualEntities = ecs.GetAll<nc::Entity>();
    ASSERT_EQ(4, actualEntities.size());

    const auto transform0 = ecs.Get<nc::Transform>(actualEntities[0]);
    const auto transform1 = ecs.Get<nc::Transform>(actualEntities[1]);
    const auto transform2 = ecs.Get<nc::Transform>(actualEntities[2]);
    const auto transform3 = ecs.Get<nc::Transform>(actualEntities[3]);
    ASSERT_NE(nullptr, transform0);
    ASSERT_NE(nullptr, transform1);
    ASSERT_NE(nullptr, transform2);
    ASSERT_NE(nullptr, transform3);

    EXPECT_EQ(nc::Entity::Null(), transform0->Parent());
    EXPECT_EQ(actualEntities[0], transform1->Parent());
    EXPECT_EQ(actualEntities[1], transform2->Parent());
    EXPECT_EQ(actualEntities[2], transform3->Parent());
}

TEST_F(SceneSerializationTests, RoundTrip_hasComponents_correctlyLoadsComponents)
{
    {
        const auto e1 = ecs.Emplace<nc::Entity>(nc::EntityInfo{});
        const auto e2 = ecs.Emplace<nc::Entity>(nc::EntityInfo{});
        const auto e3 = ecs.Emplace<nc::Entity>(nc::EntityInfo{});
        const auto e4 = ecs.Emplace<nc::Entity>(nc::EntityInfo{});

        ecs.Emplace<TestComponent1>(e1, 42);
        ecs.Emplace<TestComponent2>(e1, "test");
        ecs.Emplace<UnserializableTestComponent>(e1);
        ecs.Emplace<TestComponent1>(e2, 900);
        ecs.Emplace<TestComponent2>(e3, "another test");
        ecs.Emplace<UnserializableTestComponent>(e4);
    }

    auto stream = std::stringstream{};
    nc::SaveSceneFragment(stream, ecs, nc::asset::AssetMap{});

    registry.CommitPendingChanges();
    registry.Clear();

    nc::LoadSceneFragment(stream, ecs, *assetModule);

    const auto actualEntities = ecs.GetAll<nc::Entity>();
    ASSERT_EQ(4, actualEntities.size());

    ASSERT_TRUE(ecs.Contains<TestComponent1>(actualEntities[0]));
    ASSERT_TRUE(ecs.Contains<TestComponent2>(actualEntities[0]));
    ASSERT_FALSE(ecs.Contains<UnserializableTestComponent>(actualEntities[0]));

    EXPECT_EQ(42, ecs.Get<TestComponent1>(actualEntities[0])->value);
    EXPECT_EQ("test", ecs.Get<TestComponent2>(actualEntities[0])->value);

    ASSERT_TRUE(ecs.Contains<TestComponent1>(actualEntities[1]));
    ASSERT_FALSE(ecs.Contains<TestComponent2>(actualEntities[1]));
    EXPECT_EQ(900, ecs.Get<TestComponent1>(actualEntities[1])->value);

    ASSERT_FALSE(ecs.Contains<TestComponent1>(actualEntities[2]));
    ASSERT_TRUE(ecs.Contains<TestComponent2>(actualEntities[2]));
    EXPECT_EQ("another test", ecs.Get<TestComponent2>(actualEntities[2])->value);

    ASSERT_FALSE(ecs.Contains<TestComponent1>(actualEntities[3]));
    ASSERT_FALSE(ecs.Contains<TestComponent2>(actualEntities[3]));
    ASSERT_FALSE(ecs.Contains<UnserializableTestComponent>(actualEntities[3]));
}
