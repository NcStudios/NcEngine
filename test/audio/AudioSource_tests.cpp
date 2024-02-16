#include "gtest/gtest.h"
#include "ncengine/audio/AudioSource.h"

namespace nc
{
auto AcquireAudioClipAsset(const std::string&) -> AudioClipView
{
    static auto view = AudioClipView{}; // tests don't actually view clip data
    return view;
}
} // namespace nc

constexpr auto g_entity = nc::Entity{0, 0, 0};
const auto g_clip1 = std::string{"clip1.nca"};
const auto g_clip2 = std::string{"clip2.nca"};
const auto g_clip3 = std::string{"clip3.nca"};

TEST(AudioSourceTests, Play_validClipIndex_setsPlayState)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1}};
    ASSERT_FALSE(uut.IsPlaying());
    ASSERT_EQ(nc::audio::NullClipIndex, uut.GetRecentClipIndex());
    uut.Play(0);
    EXPECT_TRUE(uut.IsPlaying());
    EXPECT_EQ(0, uut.GetRecentClipIndex());
}

TEST(AudioSourceTests, Play_invalidClipIndex_throws)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1}};
    EXPECT_THROW(uut.Play(1), nc::NcError);
}

TEST(AudioSourceTests, PlayNext_hasPreviousState_advancesRoundRobin)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1, g_clip2, g_clip3}};
    uut.Play(0);
    uut.PlayNext();
    EXPECT_TRUE(uut.IsPlaying());
    EXPECT_EQ(1, uut.GetRecentClipIndex());
    uut.PlayNext();
    EXPECT_TRUE(uut.IsPlaying());
    EXPECT_EQ(2, uut.GetRecentClipIndex());
    uut.PlayNext();
    EXPECT_TRUE(uut.IsPlaying());
    EXPECT_EQ(0, uut.GetRecentClipIndex());
}

TEST(AudioSourceTests, PlayNext_noPreviousState_startsFromBeginning)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1}};
    uut.PlayNext();
    EXPECT_TRUE(uut.IsPlaying());
    EXPECT_EQ(0, uut.GetRecentClipIndex());
}

TEST(AudioSourceTests, PlayNext_empty_throws)
{
    auto uut = nc::audio::AudioSource{g_entity, {}};
    EXPECT_THROW(uut.PlayNext(), nc::NcError);
}

TEST(AudioSourceTests, Stop_setsPlayState)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1}};
    uut.Play(0);
    uut.Stop();
    EXPECT_FALSE(uut.IsPlaying());
    EXPECT_EQ(0, uut.GetRecentClipIndex());
}

TEST(AudioSourceTests, Stop_notPlaying_preservesPlayState)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1}};
    uut.Stop();
    EXPECT_FALSE(uut.IsPlaying());
    EXPECT_EQ(nc::audio::NullClipIndex, uut.GetRecentClipIndex());
}

TEST(AudioSourceTests, AddClip_updatesClipsAndPaths)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1}};
    const auto actualIndex = uut.AddClip(g_clip2);
    ASSERT_EQ(1, actualIndex);
    EXPECT_EQ(2, uut.GetClips().size());
    const auto paths = uut.GetAssetPaths();
    ASSERT_EQ(2, paths.size());
    EXPECT_EQ(g_clip2, paths[actualIndex]);
}

TEST(AudioSourceTests, SetClip_validClipIndex_replacesClip)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1, g_clip2}};
    uut.SetClip(0, g_clip3);
    EXPECT_EQ(2, uut.GetClips().size());
    const auto paths = uut.GetAssetPaths();
    ASSERT_EQ(2, paths.size());
    EXPECT_EQ(g_clip3, paths[0]);
}

TEST(AudioSourceTests, SetClip_invalidClipIndex_throws)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1, g_clip2}};
    EXPECT_THROW(uut.SetClip(2, g_clip3), nc::NcError);
}

TEST(AudioSourceTests, SetClip_replacesLastPlayedClip_resetsPlayState)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1, g_clip2}};
    uut.Play(0);
    uut.SetClip(0, g_clip3);
    EXPECT_FALSE(uut.IsPlaying());
    EXPECT_EQ(nc::audio::NullClipIndex, uut.GetRecentClipIndex());
}

TEST(AudioSourceTests, RemoveClip_validClipIndex_removesClip)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1, g_clip2, g_clip3}};
    uut.RemoveClip(0);
    ASSERT_EQ(2, uut.GetClips().size());
    ASSERT_EQ(2, uut.GetAssetPaths().size());
    EXPECT_EQ(g_clip2, uut.GetAssetPaths()[0]);
    EXPECT_EQ(g_clip3, uut.GetAssetPaths()[1]);
    uut.RemoveClip(1);
    ASSERT_EQ(1, uut.GetClips().size());
    ASSERT_EQ(1, uut.GetAssetPaths().size());
    EXPECT_EQ(g_clip2, uut.GetAssetPaths()[0]);
    uut.RemoveClip(0);
    ASSERT_EQ(0, uut.GetClips().size());
    ASSERT_EQ(0, uut.GetAssetPaths().size());
}

TEST(AudioSourceTests, RemoveClip_invalidClipIndex_throws)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1}};
    EXPECT_THROW(uut.RemoveClip(2), nc::NcError);
}

TEST(AudioSourceTests, RemoveClip_removesLastPlayedClip_resetsPlayState)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1}};
    uut.Play(0);
    uut.RemoveClip(0);
    EXPECT_FALSE(uut.IsPlaying());
    EXPECT_EQ(nc::audio::NullClipIndex, uut.GetRecentClipIndex());
}

TEST(AudioSourceTests, FlagGettersAndSetters_updateProperties)
{
    auto uut = nc::audio::AudioSource{g_entity, {g_clip1}};

    EXPECT_FALSE(uut.IsSpatial());
    uut.SetSpatial(true);
    EXPECT_TRUE(uut.IsSpatial());
    uut.SetSpatial(false);
    EXPECT_FALSE(uut.IsSpatial());

    EXPECT_FALSE(uut.IsLooping());
    uut.SetLooping(true);
    EXPECT_TRUE(uut.IsLooping());
    uut.SetLooping(false);
    EXPECT_FALSE(uut.IsLooping());
}
