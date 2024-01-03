#include "gtest/gtest.h"
#include "serialize/SceneSerialization.h"
#include "ncengine/scene/SceneFragment.h"

#include <sstream>

TEST(SceneSerializationTests, Deserialize_onlyHasHeader_succeeds)
{
    auto stream = std::stringstream{};
    auto header = nc::SceneFragmentHeader{};
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    auto actual = nc::SceneFragment{};
    EXPECT_NO_THROW(nc::Deserialize(stream, actual));
}

TEST(SceneSerializationTests, Deserialize_badMagicNumber_throws)
{
    const auto badMagicNumber = nc::g_sceneFragmentMagicNumber + 1;
    auto stream = std::stringstream{};
    stream.write(reinterpret_cast<const char*>(&badMagicNumber), sizeof(badMagicNumber));
    stream.write(reinterpret_cast<const char*>(&nc::g_currentSceneFragmentVersion), sizeof(nc::g_currentSceneFragmentVersion));
    auto actual = nc::SceneFragment{};
    EXPECT_THROW(nc::Deserialize(stream, actual), nc::NcError);
}

TEST(SceneSerializationTests, Deserialize_badVersion_throws)
{
    const auto unsupportedVersion = nc::g_currentSceneFragmentVersion + 1;
    auto stream = std::stringstream{};
    stream.write(reinterpret_cast<const char*>(&nc::g_sceneFragmentMagicNumber), sizeof(nc::g_sceneFragmentMagicNumber));
    stream.write(reinterpret_cast<const char*>(&unsupportedVersion), sizeof(unsupportedVersion));
    auto actual = nc::SceneFragment{};
    EXPECT_THROW(nc::Deserialize(stream, actual), nc::NcError);
}

TEST(SceneSerializationTests, SerializeDeserialize_emptyFragment_resultingFragmentEmpty)
{
    auto expected = nc::SceneFragment{};
    auto stream = std::stringstream{};
    nc::Serialize(stream, expected);
    auto actual = nc::SceneFragment{};
    nc::Deserialize(stream, actual);
    EXPECT_TRUE(actual.assets.empty());
    EXPECT_TRUE(actual.entities.empty());
}

TEST(SceneSerializationTests, SerializeDeserialize_nonEmptyFragment_preservesValues)
{
    auto expected = nc::SceneFragment
    {
        .assets = nc::asset::AssetMap
        {
            {nc::asset::AssetType::AudioClip, std::vector<std::string>{"clip"}},
            {nc::asset::AssetType::Mesh,      std::vector<std::string>{"mesh1", "mesh2", "mesh3"}}
        },
        .entities = std::vector<nc::FragmentEntityInfo>
        {
            { 0u, nc::EntityInfo{.position = nc::Vector3::Up(), .layer = nc::Entity::layer_type{2}} },
            { 1u, nc::EntityInfo{.tag = "Second", .flags = nc::Entity::Flags::Static} },
            { 2u, nc::EntityInfo{.scale = nc::Vector3::Splat(3.0f), .flags = nc::Entity::Flags::Persistent} }
        }
    };

    auto stream = std::stringstream{};
    nc::Serialize(stream, expected);
    auto actual = nc::SceneFragment{};
    nc::Deserialize(stream, actual);

    EXPECT_EQ(expected.assets.size(), actual.assets.size());
    EXPECT_TRUE(std::ranges::equal(expected.assets.at(nc::asset::AssetType::AudioClip),
                                   actual.assets.at(nc::asset::AssetType::AudioClip)));
    EXPECT_TRUE(std::ranges::equal(expected.assets.at(nc::asset::AssetType::Mesh),
                                   actual.assets.at(nc::asset::AssetType::Mesh)));

    EXPECT_EQ(expected.entities.size(), actual.entities.size());
    for (const auto& [expectedInfo, actualInfo] : std::views::zip(expected.entities, actual.entities))
    {
        EXPECT_EQ(expectedInfo.fragmentId, actualInfo.fragmentId);
        EXPECT_EQ(expectedInfo.info.position, actualInfo.info.position);
        EXPECT_EQ(expectedInfo.info.rotation, actualInfo.info.rotation);
        EXPECT_EQ(expectedInfo.info.scale, actualInfo.info.scale);
        EXPECT_EQ(expectedInfo.info.tag, actualInfo.info.tag);
        EXPECT_EQ(expectedInfo.info.layer, actualInfo.info.layer);
        EXPECT_EQ(expectedInfo.info.flags, actualInfo.info.flags);
    }
}
