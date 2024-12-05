#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/PostProcessSubsystem.h"
#include "ncengine/graphics/GraphicsUtility.h"
#include "ncutility/NcError.h"

TEST(PostProcessSubsystemTest, Constructor_initializesAllToDisabled)
{
    auto uut = nc::graphics::PostProcessSubsystem{};
    const auto expectedEffects = nc::GetPostProcessEffectIds();
    for (const auto id : expectedEffects)
    {
        auto enabled = false;
        EXPECT_NO_THROW(enabled = uut.IsEnabled(id));
        EXPECT_FALSE(enabled);
    }
}

TEST(PostProcessSubsystemTest, SetEnabled_togglesEffectState)
{
    auto uut = nc::graphics::PostProcessSubsystem{};
    const auto effectIds = nc::GetPostProcessEffectIds();
    for (const auto id : effectIds)
    {
        EXPECT_FALSE(uut.IsEnabled(id));
        uut.SetEnabled(id, true);
        EXPECT_TRUE(uut.IsEnabled(id));
        uut.SetEnabled(id, false);
        EXPECT_FALSE(uut.IsEnabled(id));
    }
}

TEST(PostProcessSubsystemTest, GetProperties_getsPropertiesIfExist)
{
    auto uut = nc::graphics::PostProcessSubsystem{};
    const auto effectIds = nc::GetPostProcessEffectIds();
    for (const auto effectId : effectIds)
    {
        const auto passIds = nc::GetPostProcessEffectPassFlags(effectId);
        for (const auto passId : passIds)
        {
            if (nc::PassHasProperties(passId))
            {
                EXPECT_NO_THROW(uut.GetProperties(effectId, passId));
            }
            else
            {
                EXPECT_THROW(uut.GetProperties(effectId, passId), nc::NcError);
            }
        }
    }
}

TEST(PostProcessSubsystemTest, SetProperties_validCall_updatesState)
{
    auto uut = nc::graphics::PostProcessSubsystem{};
    constexpr auto effect = nc::MoebiusEffectId;
    constexpr auto pass = nc::PostProcessPassFlag::Outline;
    ASSERT_TRUE(nc::PassHasProperties(pass));

    const auto expected = nc::OutlinePassProperties{
        .color = nc::Vector3{1.0f, 2.0f, 3.0f},
        .width = 3.0f
    };

    uut.SetProperties(effect, pass, expected);
    const auto& properties = uut.GetProperties(effect, pass);
    const auto& actual = std::get<nc::OutlinePassProperties>(properties);
    EXPECT_EQ(expected.color, actual.color);
    EXPECT_EQ(expected.width, actual.width);
}

TEST(PostProcessSubsystemTest, SetProperties_doesNotHaveProperties_throws)
{
    auto uut = nc::graphics::PostProcessSubsystem{};
    const auto effectIds = nc::GetPostProcessEffectIds();
    for (const auto effectId : effectIds)
    {
        const auto passIds = nc::GetPostProcessEffectPassFlags(effectId);
        for (const auto passId : passIds)
        {
            if (!nc::PassHasProperties(passId))
            {
                EXPECT_THROW(uut.GetProperties(effectId, passId), nc::NcError);
            }
        }
    }
}

TEST(PostProcessSubsystemTest, BuildState_reportsEnabledAndDisabled)
{
    auto uut = nc::graphics::PostProcessSubsystem{};

    // empty succeeds
    auto state = uut.BuildState();
    EXPECT_EQ(0, state.toggledEffects.size());
    EXPECT_EQ(0, state.modifiedProperties.size());

    // reports enable event
    uut.SetEnabled(nc::MoebiusEffectId, true);
    state = uut.BuildState();
    EXPECT_EQ(1, state.toggledEffects.size());
    EXPECT_EQ(0, state.modifiedProperties.size());
    const auto& enableEvent = state.toggledEffects.at(0);
    EXPECT_EQ(nc::MoebiusEffectId, enableEvent.effectId);
    EXPECT_EQ(nc::MoebiusEffectPassFlags, enableEvent.passes);
    EXPECT_TRUE(enableEvent.enabled);

    // enable event cleared
    state = uut.BuildState();
    EXPECT_EQ(0, state.toggledEffects.size());
    EXPECT_EQ(0, state.modifiedProperties.size());

    // reports disable event
    uut.SetEnabled(nc::MoebiusEffectId, false);
    state = uut.BuildState();
    EXPECT_EQ(1, state.toggledEffects.size());
    EXPECT_EQ(0, state.modifiedProperties.size());
    const auto& disableEvent = state.toggledEffects.at(0);
    EXPECT_EQ(nc::MoebiusEffectId, disableEvent.effectId);
    EXPECT_EQ(nc::MoebiusEffectPassFlags, disableEvent.passes);
    EXPECT_FALSE(disableEvent.enabled);

    // disable event cleared
    state = uut.BuildState();
    EXPECT_EQ(0, state.toggledEffects.size());
    EXPECT_EQ(0, state.modifiedProperties.size());
}

TEST(PostProcessSubsystemTest, BuildState_reportsPropertyModification)
{
    auto uut = nc::graphics::PostProcessSubsystem{};

    // empty succeeds
    auto state = uut.BuildState();
    EXPECT_EQ(0, state.toggledEffects.size());
    EXPECT_EQ(0, state.modifiedProperties.size());

    // reports property modification
    const auto expectedProperties = nc::OutlinePassProperties{nc::Vector3::Up(), 10.0f};
    uut.SetProperties(nc::MoebiusEffectId, nc::PostProcessPassFlag::Outline, expectedProperties);
    state = uut.BuildState();
    EXPECT_EQ(0, state.toggledEffects.size());
    EXPECT_EQ(1, state.modifiedProperties.size());
    const auto& modifyEvent = state.modifiedProperties.at(0);
    EXPECT_EQ(nc::MoebiusEffectId, modifyEvent.effectId);
    EXPECT_EQ(nc::PostProcessPassFlag::Outline, modifyEvent.pass);
    const auto& actualProperties = std::get<nc::OutlinePassProperties>(modifyEvent.properties);
    EXPECT_EQ(expectedProperties.color, actualProperties.color);
    EXPECT_EQ(expectedProperties.width, actualProperties.width);

    // modify event cleared
    state = uut.BuildState();
    EXPECT_EQ(0, state.toggledEffects.size());
    EXPECT_EQ(0, state.modifiedProperties.size());
}
