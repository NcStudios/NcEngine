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
            if (nc::HasProperties(passId))
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
    constexpr auto pass = nc::PostProcessPass::Outline;
    ASSERT_TRUE(nc::HasProperties(pass));

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
            if (!nc::HasProperties(passId))
            {
                EXPECT_THROW(uut.GetProperties(effectId, passId), nc::NcError);
            }
        }
    }
}
