#include "gtest/gtest.h"
#include "ncengine/audio/AudioBuffer.h"

class AudioBufferTest : public ::testing::Test
{
    protected:
        static constexpr uint32_t FramesPerBuffer = 128u;
        nc::audio::AudioBuffer uut{FramesPerBuffer};
};

TEST_F(AudioBufferTest, constructor_allBuffersStartStale)
{
    for (auto i = 0u; i < nc::audio::AudioBuffer::BufferSlices; ++i)
    {
        const auto actual = uut.AcquireStaleBuffer();
        EXPECT_NE(actual.data, nullptr);
        EXPECT_LT(actual.index, nc::audio::AudioBuffer::BufferSlices);
    }

    const auto nonexistent = uut.AcquireStaleBuffer();
    EXPECT_EQ(nonexistent.data, nullptr);
    EXPECT_EQ(nonexistent.index, nc::audio::BufferSlice::NullIndex);
}

TEST_F(AudioBufferTest, trivialGetters_returnExpectedValues)
{
    EXPECT_EQ(uut.FramesPerBuffer(), FramesPerBuffer);
    EXPECT_EQ(uut.SamplesPerBuffer(), FramesPerBuffer * nc::audio::AudioBuffer::OutputChannelCount);
    EXPECT_EQ(uut.BytesPerBuffer(), uut.SamplesPerBuffer() * sizeof(double));
}

TEST_F(AudioBufferTest, MarkBufferReady_makesAvailableAsReadyBuffer)
{
    auto original = uut.AcquireStaleBuffer();
    ASSERT_NE(original.data, nullptr);
    std::fill(original.data, original.data + uut.SamplesPerBuffer(), 42.0);
    uut.MarkBufferReady(original);

    auto ready = uut.AcquireReadyBuffer();
    EXPECT_EQ(ready.data, original.data);
    EXPECT_EQ(ready.index, original.index);
    for (auto i = 0u; i < uut.SamplesPerBuffer(); ++i)
    {
        EXPECT_EQ(42.0, ready.data[i]);
    }
}

TEST_F(AudioBufferTest, MarkBufferStale_returnsToStaleQueue)
{
    const auto original = uut.AcquireStaleBuffer();
    ASSERT_NE(original.data, nullptr);

    while (uut.AcquireStaleBuffer().data != nullptr); // exhaust stale buffers
    uut.MarkBufferReady(original);

    const auto actual = uut.AcquireReadyBuffer();
    EXPECT_EQ(original.data, actual.data);
    EXPECT_EQ(original.index, actual.index);
}

TEST_F(AudioBufferTest, Resize_resetsBuffersAndCapacity)
{
    const auto newFrames = FramesPerBuffer * 2u;
    uut.Resize(newFrames);

    EXPECT_EQ(uut.FramesPerBuffer(), newFrames);
    EXPECT_EQ(uut.SamplesPerBuffer(), newFrames * nc::audio::AudioBuffer::OutputChannelCount);

    for (auto i = 0u; i < nc::audio::AudioBuffer::BufferSlices; ++i)
    {
        auto slice = uut.AcquireStaleBuffer();
        EXPECT_NE(slice.data, nullptr);
        EXPECT_EQ(slice.index, i);
    }

    auto slice = uut.AcquireStaleBuffer();
    EXPECT_EQ(slice.data, nullptr);
}

TEST_F(AudioBufferTest, Clear_marksAllStale)
{
    for (auto i = 0u; i < nc::audio::AudioBuffer::BufferSlices; ++i)
    {
        auto slice = uut.AcquireStaleBuffer();
        ASSERT_NE(slice.data, nullptr);
        uut.MarkBufferReady(slice);
    }

    uut.Clear();

    EXPECT_EQ(nullptr, uut.AcquireReadyBuffer().data);

    for (auto i = 0u; i < nc::audio::AudioBuffer::BufferSlices; ++i)
    {
        auto slice = uut.AcquireStaleBuffer();
        EXPECT_NE(slice.data, nullptr);
    }
}
