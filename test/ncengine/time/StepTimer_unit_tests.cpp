#include "gtest/gtest.h"
#include "time/StepTimer.h"

#include <algorithm>
#include <vector>

void TestUpdate(float) {}

TEST(StepTimerTests, GetDeltaTime_fixedStep_alwayReturnsFixedStep)
{
    auto deltaTimeValues = std::vector<float>{};
    auto update = [&deltaTimeValues](float dt) { deltaTimeValues.push_back(dt); };
    auto uut = nc::time::StepTimer{0.01667f, 0.1f};
    auto accumulatedFrames = 0ull;
    while (accumulatedFrames < 10ull)
    {
        if (uut.Tick(update))
            accumulatedFrames += uut.GetFramesThisTick();
    }

    ASSERT_EQ(10ull, deltaTimeValues.size());
    auto eliminated = std::ranges::unique(deltaTimeValues);
    EXPECT_EQ(9ull, eliminated.size());
}

TEST(StepTimerTests, GetDeltaTime_variableStep_returnsDifferentValues)
{
    auto deltaTimeValues = std::vector<float>{};
    auto update = [&deltaTimeValues](float dt) { deltaTimeValues.push_back(dt); };
    auto uut = nc::time::StepTimer{0.1f};
    auto accumulatedFrames = 0ull;
    while (accumulatedFrames < 10ull)
    {
        uut.Tick(update);
        ++accumulatedFrames;
    }

    ASSERT_EQ(10ull, deltaTimeValues.size());
    auto eliminated = std::ranges::unique(deltaTimeValues);
    EXPECT_NE(9ull, eliminated.size()); // could have some unique values, but unlikely all 10 are the same
}

TEST(StepTimerTests, GetFrameCount_fixedStep_returnsTotalFramesRun)
{
    auto uut = nc::time::StepTimer{0.01667f, 0.1f};
    auto accumulatedFrames = 0ull;
    while (accumulatedFrames < 5ull)
    {
        if (uut.Tick(TestUpdate))
            accumulatedFrames += uut.GetFramesThisTick();
    }

    EXPECT_EQ(accumulatedFrames, uut.GetFrameCount());
}

TEST(StepTimerTests, GetFrameCount_variableStep_returnsTotalFramesRun)
{
    auto uut = nc::time::StepTimer{0.1f};
    auto accumulatedFrames = 0ull;
    while (accumulatedFrames < 5ull)
    {
        uut.Tick(TestUpdate);
        ++accumulatedFrames;
    }

    EXPECT_EQ(accumulatedFrames, uut.GetFrameCount());
}

TEST(StepTimerTests, Tick_fixedStep_invokesCallbackOnTimeStepExceeded)
{
    const auto expectedInvocations = 5ull;
    auto invocations = 0ull;
    auto update = [&invocations](float) { ++invocations; };
    auto uut = nc::time::StepTimer{0.01667f, 0.1f};
    while (uut.GetFrameCount() < expectedInvocations)
        uut.Tick(update);

    EXPECT_EQ(expectedInvocations, invocations);
}

TEST(StepTimerTests, Tick_variableStep_invokesCallbackEachTick)
{
    const auto expectedInvocations = 5ull;
    auto invocations = 0ull;
    auto update = [&invocations](float) { ++invocations; };
    auto uut = nc::time::StepTimer{0.1f};
    while (uut.GetFrameCount() < expectedInvocations)
        uut.Tick(update);

    EXPECT_EQ(expectedInvocations, invocations);
}

TEST(StepTimerTests, Tick_fixedStepWithLongDelay_clampsToMaxDeltaTime)
{
    auto uut = nc::time::StepTimer{0.01667f, 0.1f};
    while (!uut.Tick(TestUpdate)) {}
    std::this_thread::sleep_for(std::chrono::milliseconds{200});
    uut.Tick(TestUpdate);
    // since reported dt is also fixed, instead check we didn't exceed 0.1 seconds worth of frames
    const auto expectedMaxFrames = static_cast<uint64_t>(0.1f / 0.01667f);
    EXPECT_GE(expectedMaxFrames, uut.GetFramesThisTick());
}

TEST(StepTimerTests, Tick_variableStepWithLongDelay_clampsToMaxDeltaTime)
{
    auto uut = nc::time::StepTimer{0.1f};
    while (!uut.Tick(TestUpdate)) {}
    std::this_thread::sleep_for(std::chrono::milliseconds{200});
    uut.Tick(TestUpdate);
    EXPECT_GE(0.1f, uut.GetDeltaTime());
}

TEST(StepTimerTests, Reset_resetsTimeStamps)
{
    auto uut = nc::time::StepTimer{0.01667f, 0.1f};
    while (!uut.Tick(TestUpdate)) {}
    std::this_thread::sleep_for(std::chrono::milliseconds{20});
    uut.Reset();
    EXPECT_FALSE(uut.Tick(TestUpdate)); // dt should be near 0, not 20ms
}
