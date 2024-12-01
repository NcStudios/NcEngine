#include "gtest/gtest.h"
#include "ncengine/graphics/PostProcess.h"
#include "ncengine/graphics/GraphicsUtility.h"
#include "ncutility/NcError.h"

TEST(PostProcessTest, HasProperties_succeedsForAllPasses)
{
    for (const auto pass : nc::GetPostProcessPassFlags())
    {
        EXPECT_NO_THROW(nc::PassHasProperties(pass));
    }
}

TEST(PostProcessTest, MakeDefaultPassProperties_succeedsForAllPasses)
{
    for (const auto pass : nc::GetPostProcessPassFlags())
    {
        EXPECT_NO_THROW(nc::MakeDefaultPassProperties(pass));
    }
}

TEST(PostProcessTest, MakeDefaultPassProperties_invalidPass_throws)
{
    EXPECT_THROW(nc::MakeDefaultPassProperties(nc::PostProcessPass::None), nc::NcError);
}

TEST(PostProcessTest, MakeDefaultPassProperties_ifPassHasProperties_returnsNonEmptyResult)
{
    auto isEmptyProperties = [](auto&& properties)
    {
        return std::visit([](auto&& unpacked){
            using T = std::decay_t<decltype(unpacked)>;
            if constexpr (std::same_as<T, nc::EmptyPassProperties>)
                return true;
            else
                return false;
        }, properties);
    };

    for (const auto pass : nc::GetPostProcessPassFlags())
    {
        const auto properties = nc::MakeDefaultPassProperties(pass);
        const auto isEmpty = isEmptyProperties(properties);
        if (nc::PassHasProperties(pass))
        {
            EXPECT_FALSE(isEmpty);
        }
        else
        {
            EXPECT_TRUE(isEmpty);
        }
    }
}

