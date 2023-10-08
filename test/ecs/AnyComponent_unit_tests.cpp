#include "ncengine/ecs/AnyComponent.h"
#include "gtest/gtest.h"

struct TestComponent{};

TEST(AnyComponentTest, OperatorBool_hasValue_returnsTrue)
{
    auto component = TestComponent{};
    auto handler = nc::ComponentHandler<TestComponent>{};
    const auto uut = nc::AnyComponent{&component, &handler};
    EXPECT_TRUE(static_cast<bool>(uut));
}

TEST(AnyComponentTest, OperatorBool_nullValue_returnsFalse)
{
    const auto uut = nc::AnyComponent{};
    EXPECT_FALSE(static_cast<bool>(uut));
}

TEST(AnyComponentTest, DrawUI_validCall_dispatchesToHandler)
{
    auto numCalls = 0;
    auto component = TestComponent{};
    auto handler = nc::ComponentHandler<TestComponent>
    {
        .drawUI = [&numCalls](TestComponent&) { ++numCalls; }
    };

    auto uut = nc::AnyComponent{&component, &handler};
    uut.DrawUI();
    uut.DrawUI();
    EXPECT_EQ(2, numCalls);
}
