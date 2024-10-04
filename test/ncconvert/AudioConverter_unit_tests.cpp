#include "gtest/gtest.h"
#include "CollateralAudio.h"
#include "converters/AudioConverter.h"
#include "ncasset/Assets.h"

#include <algorithm>

TEST(AudioConverterTest, ImportAudioClip_convertsToNca)
{
    namespace test_data = collateral::sine;
    auto uut = nc::convert::AudioConverter{};
    const auto actual = uut.ImportAudioClip(test_data::filePath);

    EXPECT_EQ(actual.samplesPerChannel, test_data::samplesPerChannel);
    ASSERT_EQ(actual.leftChannel.size(), test_data::leftChannel.size());
    ASSERT_EQ(actual.rightChannel.size(), test_data::rightChannel.size());
    EXPECT_TRUE(std::equal(actual.leftChannel.cbegin(),
                           actual.leftChannel.cend(),
                           test_data::leftChannel.cbegin()));
    EXPECT_TRUE(std::equal(actual.rightChannel.cbegin(),
                           actual.rightChannel.cend(),
                           test_data::rightChannel.cbegin()));
}
