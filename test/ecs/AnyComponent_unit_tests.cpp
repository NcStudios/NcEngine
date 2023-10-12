#include "ncengine/ecs/AnyComponent.h"
#include "gtest/gtest.h"

struct TestComponent{};
struct AlternativeComponent{};

TEST(AnyComponentTest, Constructor_nullParams_throws)
{
    auto actualComponent = TestComponent{};
    auto actualHandler = nc::ComponentHandler<TestComponent>{};
    TestComponent* nullComponent = nullptr;
    nc::ComponentHandler<TestComponent>* nullHandler = nullptr;
    EXPECT_THROW(nc::AnyComponent(nullComponent, nullHandler), nc::NcError);
    EXPECT_THROW(nc::AnyComponent(nullComponent, &actualHandler), nc::NcError);
    EXPECT_THROW(nc::AnyComponent(&actualComponent, nullHandler), nc::NcError);
}

TEST(AnyComponentTest, CopyOperations_objectsValidAndEqual)
{
    // Test valid object copy
    {
        auto component = TestComponent{};
        auto handler = nc::ComponentHandler<TestComponent>{};
        const auto source = nc::AnyComponent{&component, &handler};
        auto copy = source; // copy construct
        EXPECT_TRUE(copy);
        EXPECT_TRUE(source);
        EXPECT_EQ(source, copy);
        copy = source; // copy assign
        EXPECT_TRUE(copy);
        EXPECT_TRUE(source);
        EXPECT_EQ(source, copy);
    }

    // Test null object copy
    {
        auto source = nc::AnyComponent{};
        auto copy = nc::AnyComponent{};
        EXPECT_FALSE(source);
        EXPECT_FALSE(copy);
        EXPECT_EQ(source, copy);
        copy = source;
        EXPECT_FALSE(source);
        EXPECT_FALSE(copy);
        EXPECT_EQ(source, copy);
    }
}

TEST(AnyComponentTest, MoveOperations_objectsValid)
{
    // Test valid object move
    {
        auto component = TestComponent{};
        auto handler = nc::ComponentHandler<TestComponent>{};
        auto source = nc::AnyComponent{&component, &handler};
        // Keep a backup for comparison since we're moving. Its ptrs being moved, so
        // maybe not necessary, but feels weird comparing to a moved-from value.
        auto backup = source;
        auto moved = std::move(source); // move construct
        EXPECT_TRUE(moved);
        EXPECT_TRUE(source);
        EXPECT_EQ(backup, moved);
        moved = nc::AnyComponent{&component, &handler}; // move assign
        EXPECT_TRUE(moved);
        EXPECT_EQ(backup, moved);
    }

    // Test null object move
    {
        auto source = nc::AnyComponent{};
        auto backup = source; // overkill at this point
        auto moved = std::move(source);
        EXPECT_FALSE(source);
        EXPECT_FALSE(moved);
        EXPECT_EQ(backup, moved);
        moved = nc::AnyComponent{};
        EXPECT_FALSE(source);
        EXPECT_EQ(backup, moved);
    }
}

TEST(AnyComponentTest, EqualityOperators_equal_compareEqual)
{
    auto component = TestComponent{};
    auto handler = nc::ComponentHandler<TestComponent>{};
    const auto lhs = nc::AnyComponent{&component, &handler};
    const auto rhs = nc::AnyComponent{&component, &handler};
    EXPECT_TRUE(lhs == rhs);
    EXPECT_FALSE(lhs != rhs);
}

TEST(AnyComponentTest, EqualityOperators_notEqual_compareNotEqual)
{
    auto component1 = TestComponent{};
    auto component2 = TestComponent{};
    auto handler = nc::ComponentHandler<TestComponent>{};
    const auto lhs = nc::AnyComponent{&component1, &handler};
    const auto rhs = nc::AnyComponent{&component2, &handler};
    EXPECT_FALSE(lhs == rhs);
    EXPECT_TRUE(lhs != rhs);
}

TEST(AnyComponentTest, EqualityOperators_differentTypes_compareNotEqual)
{
    auto component1 = TestComponent{};
    auto component2 = AlternativeComponent{};
    auto handler1 = nc::ComponentHandler<TestComponent>{};
    auto handler2 = nc::ComponentHandler<AlternativeComponent>{};
    const auto lhs = nc::AnyComponent{&component1, &handler1};
    const auto rhs = nc::AnyComponent{&component2, &handler2};
    EXPECT_FALSE(lhs == rhs);
    EXPECT_TRUE(lhs != rhs);
}

TEST(AnyComponentTest, EqualityOperators_nullObjects_compareEqual)
{
    const auto lhs = nc::AnyComponent{};
    const auto rhs = nc::AnyComponent{};
    EXPECT_TRUE(lhs == rhs);
}

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

TEST(AnyComponentTest, Name_nameSet_retrievesValue)
{
    constexpr auto expected = "test";
    auto component = TestComponent{};
    auto handler = nc::ComponentHandler<TestComponent>{ .name = expected };
    auto uut = nc::AnyComponent{&component, &handler};
    EXPECT_EQ(expected, uut.Name());
}

TEST(AnyComponentTest, HasDrawUI_funcSet_returnsTrue)
{
    auto component = TestComponent{};
    auto handler = nc::ComponentHandler<TestComponent>{ .drawUI = [](TestComponent&){} };
    auto uut = nc::AnyComponent{&component, &handler};
    EXPECT_TRUE(uut.HasDrawUI());
}

TEST(AnyComponentTest, HasDrawUI_funcNotSet_returnsFalse)
{
    auto component = TestComponent{};
    auto handler = nc::ComponentHandler<TestComponent>{ .drawUI = nullptr };
    auto uut = nc::AnyComponent{&component, &handler};
    EXPECT_FALSE(uut.HasDrawUI());
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

TEST(AnyComponentTest, DrawUI_nullFunc_succeeds)
{
    auto component = TestComponent{};
    auto handler = nc::ComponentHandler<TestComponent>{ .drawUI = nullptr };
    auto uut = nc::AnyComponent{&component, &handler};
    EXPECT_FALSE(uut.HasDrawUI());
    EXPECT_NO_THROW(uut.DrawUI());
}

TEST(AnyComponentTest, DrawUI_nullObject_throws)
{
    auto uut = nc::AnyComponent{};
    EXPECT_THROW(uut.DrawUI(), nc::NcError);
}
