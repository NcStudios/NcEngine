#include "gtest/gtest.h"
#include "ncengine/scene/SceneFragment.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Registry.h"

#include <set>
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

class SceneFragmentTests : public ::testing::Test
{
    public:
        static constexpr auto maxEntities = 10ull;
        nc::Registry legacyRegistry; // unfortunately we need this around for ActiveRegistry() usage inside Transform
        nc::ecs::ComponentRegistry& registry;
        nc::ecs::Ecs ecs;
        std::unique_ptr<nc::asset::NcAsset> assetModule;

        SceneFragmentTests()
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

        ~SceneFragmentTests()
        {
            legacyRegistry.Reset();
        }
};

TEST_F(SceneFragmentTests, SaveSceneFragment_emptyGameState_succeeds)
{
    const auto actual = nc::SaveSceneFragment(ecs, *assetModule);
    EXPECT_TRUE(actual.assets.empty());
    EXPECT_TRUE(actual.entities.empty());
}

TEST_F(SceneFragmentTests, SaveSceneFragment_hasLoadedAssets_savesAssets)
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
    const auto actual = nc::SaveSceneFragment(ecs, *assetModule);
    ASSERT_FALSE(actual.assets.empty());
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::AudioClip),
                                   actual.assets.at(nc::asset::AssetType::AudioClip)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::CubeMap),
                                   actual.assets.at(nc::asset::AssetType::CubeMap)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::ConcaveCollider),
                                   actual.assets.at(nc::asset::AssetType::ConcaveCollider)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::HullCollider),
                                   actual.assets.at(nc::asset::AssetType::HullCollider)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::Mesh),
                                   actual.assets.at(nc::asset::AssetType::Mesh)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::SkeletalAnimation),
                                   actual.assets.at(nc::asset::AssetType::SkeletalAnimation)));
    EXPECT_TRUE(std::ranges::equal(expectedAssets.at(nc::asset::AssetType::Texture),
                                   actual.assets.at(nc::asset::AssetType::Texture)));
}

TEST_F(SceneFragmentTests, SaveSceneFragment_assignsUniqueFragmentIds)
{
    ecs.Emplace<nc::Entity>(nc::EntityInfo{});
    ecs.Emplace<nc::Entity>(nc::EntityInfo{});
    ecs.Emplace<nc::Entity>(nc::EntityInfo{});
    ecs.Emplace<nc::Entity>(nc::EntityInfo{});

    const auto actual = nc::SaveSceneFragment(ecs, *assetModule);
    EXPECT_EQ(4, actual.entities.size());
    const auto actualIds = std::views::transform(actual.entities, [](auto&& info) { return info.fragmentId; });
    const auto uniqueIds = std::set<uint32_t>{std::ranges::cbegin(actualIds), std::ranges::cend(actualIds)};
    ASSERT_EQ(uniqueIds.size(), actualIds.size());
}

TEST_F(SceneFragmentTests, SaveSceneFragment_hasEntities_reconstructsEntityInfos)
{
    const auto expectedInfos = std::vector<nc::EntityInfo>
    {
        nc::EntityInfo{},
        nc::EntityInfo
        {
            .position = nc::Vector3::Up(),
            .rotation = nc::Quaternion::FromEulerAngles(1.0f, 2.0f, 3.0f),
            .scale = nc::Vector3{1.0f, 2.0f, 1.0f},
            .tag = "Second",
            .layer = nc::Entity::layer_type{2},
            .flags = nc::Entity::Flags::Persistent
        },
        nc::EntityInfo
        {
            .tag = "Third",
            .flags = nc::Entity::Flags::Static
        }
    };

    std::ranges::for_each(expectedInfos, [&ecs = ecs](const auto& info)
    {
        ecs.Emplace<nc::Entity>(info);
    });

    const auto fragment = nc::SaveSceneFragment(ecs, *assetModule);
    EXPECT_EQ(expectedInfos.size(), fragment.entities.size());

    for (const auto& [expected, actual] : std::views::zip(expectedInfos, fragment.entities))
    {
        EXPECT_EQ(expected.position, actual.info.position);
        EXPECT_EQ(expected.rotation, actual.info.rotation);
        EXPECT_EQ(expected.scale, actual.info.scale);
        // note: actual.info.parent has been remapped if not Entity::Null()
        EXPECT_EQ(expected.tag, actual.info.tag);
        EXPECT_EQ(expected.layer, actual.info.layer);
        EXPECT_EQ(expected.flags, actual.info.flags);
    }
}

TEST_F(SceneFragmentTests, SaveSceneFragment_hasEntityHierarchy_savesEntities)
{
    const auto e1 = ecs.Emplace<nc::Entity>(nc::EntityInfo{.tag = "1"});
    const auto e2 = ecs.Emplace<nc::Entity>(nc::EntityInfo{.parent = e1, .tag = "2"});
    const auto e3 = ecs.Emplace<nc::Entity>(nc::EntityInfo{.parent = e2, .tag = "3"});
    const auto e4 = ecs.Emplace<nc::Entity>(nc::EntityInfo{.tag = "4"});
    // Disrupt parenting order to ensure we're sorting correctly prior to saving
    ecs.Get<nc::Transform>(e1)->SetParent(e4);

    const auto actual = nc::SaveSceneFragment(ecs, *assetModule);
    EXPECT_EQ(4, actual.entities.size());
    EXPECT_EQ(actual.entities.at(0).fragmentId, actual.entities.at(1).info.parent.Index());
    EXPECT_EQ(actual.entities.at(1).fragmentId, actual.entities.at(2).info.parent.Index());
    EXPECT_EQ(actual.entities.at(2).fragmentId, actual.entities.at(3).info.parent.Index());
}

TEST_F(SceneFragmentTests, LoadSceneFragment_empty_succeeds)
{
    auto fragment = nc::SceneFragment{};
    nc::LoadSceneFragment(fragment, ecs, *assetModule);
    EXPECT_TRUE(assetModule->GetLoadedAssets().empty());
    EXPECT_TRUE(ecs.GetAll<nc::Entity>().empty());
}

TEST_F(SceneFragmentTests, LoadSceneFragment_hasAssets_loadsAssets)
{
    auto fragment = nc::SceneFragment
    {
        .assets = nc::asset::AssetMap
        {
            {nc::asset::AssetType::AudioClip,         std::vector<std::string>{"ac"}},
            {nc::asset::AssetType::CubeMap,           std::vector<std::string>{"cm1", "cm2"}},
            {nc::asset::AssetType::ConcaveCollider,   std::vector<std::string>{"cc"}},
            {nc::asset::AssetType::HullCollider,      std::vector<std::string>{"hc"}},
            {nc::asset::AssetType::Mesh,              std::vector<std::string>{"m1", "m2", "m3"}},
            {nc::asset::AssetType::SkeletalAnimation, std::vector<std::string>{"sa"}},
            {nc::asset::AssetType::Texture,           std::vector<std::string>{"t"}}
        }
    };

    nc::LoadSceneFragment(fragment, ecs, *assetModule);
    const auto actualAssets = assetModule->GetLoadedAssets();
    EXPECT_TRUE(std::ranges::equal(fragment.assets.at(nc::asset::AssetType::AudioClip),
                                   actualAssets.at(nc::asset::AssetType::AudioClip)));
    EXPECT_TRUE(std::ranges::equal(fragment.assets.at(nc::asset::AssetType::CubeMap),
                                   actualAssets.at(nc::asset::AssetType::CubeMap)));
    EXPECT_TRUE(std::ranges::equal(fragment.assets.at(nc::asset::AssetType::ConcaveCollider),
                                   actualAssets.at(nc::asset::AssetType::ConcaveCollider)));
    EXPECT_TRUE(std::ranges::equal(fragment.assets.at(nc::asset::AssetType::HullCollider),
                                   actualAssets.at(nc::asset::AssetType::HullCollider)));
    EXPECT_TRUE(std::ranges::equal(fragment.assets.at(nc::asset::AssetType::Mesh),
                                   actualAssets.at(nc::asset::AssetType::Mesh)));
    EXPECT_TRUE(std::ranges::equal(fragment.assets.at(nc::asset::AssetType::SkeletalAnimation),
                                   actualAssets.at(nc::asset::AssetType::SkeletalAnimation)));
    EXPECT_TRUE(std::ranges::equal(fragment.assets.at(nc::asset::AssetType::Texture),
                                   actualAssets.at(nc::asset::AssetType::Texture)));
}

TEST_F(SceneFragmentTests, LoadSceneFragment_hasEntities_loadsEntities)
{
    auto fragment = nc::SceneFragment
    {
        .entities = std::vector<nc::FragmentEntityInfo>
        {
            {0u, nc::EntityInfo{}},
            {1u, nc::EntityInfo
            {
                .position = nc::Vector3::Up(),
                .rotation = nc::Quaternion::FromEulerAngles(1.0f, 2.0f, 3.0f),
                .scale = nc::Vector3{1.0f, 2.0f, 1.0f},
                .tag = "Second",
                .layer = nc::Entity::layer_type{2},
                .flags = nc::Entity::Flags::Persistent
            }},
            {2u, nc::EntityInfo
            {
                .tag = "Third",
                .flags = nc::Entity::Flags::Static
            }}
        }
    };

    nc::LoadSceneFragment(fragment, ecs, *assetModule);
    const auto entities = ecs.GetAll<nc::Entity>();
    ASSERT_EQ(fragment.entities.size(), entities.size());
    for (const auto& [expected, actualEntity] : std::views::zip(fragment.entities, entities))
    {
        EXPECT_EQ(expected.info.layer, actualEntity.Layer());
        EXPECT_EQ(expected.info.flags, actualEntity.Flags());

        const auto actualTransform = ecs.Get<nc::Transform>(actualEntity);
        ASSERT_NE(nullptr, actualTransform);
        EXPECT_EQ(expected.info.position, actualTransform->LocalPosition());
        EXPECT_EQ(expected.info.rotation, actualTransform->LocalRotation());
        EXPECT_EQ(expected.info.scale, actualTransform->LocalScale());

        const auto actualTag = ecs.Get<nc::Tag>(actualEntity);
        ASSERT_NE(nullptr, actualTag);
        EXPECT_EQ(expected.info.tag, actualTag->Value());
    }
}

TEST_F(SceneFragmentTests, LoadSceneFragment_hasEntityHierarchy_setsParents)
{
    
}
