#include "gtest/gtest.h"
#include "ncengine/serialize/SceneSerialization.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Registry.h"

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
        }

        ~SceneSerializationTests()
        {
            legacyRegistry.Reset();
        }
};




// TODO: add back missing tests from scene serialization: fails with bad magic number, fails with bad version






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
        nc::EntityInfo{},
        nc::EntityInfo
        {
            .position = nc::Vector3::Up(),
            .rotation = nc::Quaternion::FromEulerAngles(1.0f, 2.0f, 3.0f),
            .scale = nc::Vector3{5.0f, 10.0f, 20.0f},
        },
        nc::EntityInfo
        {
            .tag = "last",
            .layer = nc::Entity::layer_type{2},
            .flags = nc::Entity::Flags::Static
        }
    };

    std::ranges::for_each(expectedInfos, [&ecs = ecs](const auto& info)
    {
        ecs.Emplace<nc::Entity>(info);
    });

    auto stream = std::stringstream{};
    nc::SaveSceneFragment(stream, ecs, nc::asset::AssetMap{});

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
        const auto e3 = ecs.Emplace<nc::Entity>(nc::EntityInfo{.parent = e2, .tag = "3"});
        const auto e4 = ecs.Emplace<nc::Entity>(nc::EntityInfo{.tag = "4"});
        // Disrupt parenting order to ensure we're sorting correctly prior to saving
        ecs.Get<nc::Transform>(e1)->SetParent(e4);
    }

    auto stream = std::stringstream{};
    nc::SaveSceneFragment(stream, ecs, nc::asset::AssetMap{});

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
