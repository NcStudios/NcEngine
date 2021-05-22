#include "gtest/gtest.h"
#include "ecs/Registry.h"

using namespace nc;
using namespace nc::ecs;

constexpr auto TestLayer = HandleTraits::layer_type{0u};
constexpr auto TestVersion = HandleTraits::version_type{0u};
constexpr auto TestFlags = EntityFlags::None;

constexpr auto handle0 = EntityHandle{0u, TestVersion, TestLayer, TestFlags};
constexpr auto handle1 = EntityHandle{1u, TestVersion, TestLayer, TestFlags};
constexpr auto handle2 = EntityHandle{2u, TestVersion, TestLayer, TestFlags};
constexpr auto handle3 = EntityHandle{3u, TestVersion, TestLayer, TestFlags};
constexpr auto handle4 = EntityHandle{4u, TestVersion, TestLayer, TestFlags};

struct Fake : public Component
{
    public:
        Fake(EntityHandle handle)
            : Component{handle}
        {
        }
};

TEST(Registry_unit_tests, Emplace_SequentialCalls_AddsSequentially)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Emplace(handle2);
    registry.Emplace(handle3);
    auto components = registry.View();
    ASSERT_EQ(components.size(), 3u);
    EXPECT_EQ(components[0].GetParentHandle(), handle1);
    EXPECT_EQ(components[1].GetParentHandle(), handle2);
    EXPECT_EQ(components[2].GetParentHandle(), handle3);
}

TEST(Registry_unit_tests, Emplace_AddDuplicate_Throws)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    EXPECT_THROW(registry.Emplace(handle1), std::runtime_error);
}

TEST(Registry_unit_tests, Emplace_AddDuplicateAfterRemove_Succeeds)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Remove(handle1);
    auto actual = registry.Emplace(handle1);
    EXPECT_NE(actual, nullptr);
}

TEST(Registry_unit_tests, Emplace_AddDuplicateAfterClear_Succeeds)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Clear();
    auto actual = registry.Emplace(handle1);
    EXPECT_NE(actual, nullptr);
}

TEST(Registry_unit_tests, Remove_ValidHandle_RemovesAndMaintainsPacking)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Emplace(handle2);
    registry.Emplace(handle3);
    auto result = registry.Remove(handle1);
    EXPECT_TRUE(result);
    auto components = registry.View();
    ASSERT_EQ(components.size(), 2u);
    EXPECT_EQ(components[0].GetParentHandle(), handle3);
    EXPECT_EQ(components[1].GetParentHandle(), handle2);
}

TEST(Registry_unit_tests, Remove_InvalidHandle_RegistryUnchanged)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Emplace(handle2);
    auto result = registry.Remove(handle3);
    EXPECT_FALSE(result);
    auto components = registry.View();
    ASSERT_EQ(components.size(), 2u);
    EXPECT_EQ(components[0].GetParentHandle(), handle1);
    EXPECT_EQ(components[1].GetParentHandle(), handle2);
}

TEST(Registry_unit_tests, Remove_DoubleCall_ReturnsFalse)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    auto removed = registry.Remove(handle1);
    EXPECT_TRUE(removed);
    auto actual = registry.Remove(handle1);
    EXPECT_FALSE(actual);
}

TEST(Registry_unit_tests, Remove_AfterClear_ReturnsFalse)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Clear();
    auto actual = registry.Remove(handle1);
    EXPECT_FALSE(actual);
}

TEST(Registry_unit_tests, Remove_SingularCollection_UpdatesSparseSet)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    auto result = registry.Remove(handle1);
    EXPECT_TRUE(result);
    EXPECT_FALSE(registry.Contains(handle1));
    EXPECT_EQ(registry.Get(handle1), nullptr);
}

TEST(Registry_unit_tests, Remove_FromFront_UpdatesSparseSet)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Emplace(handle2);
    registry.Emplace(handle3);
    auto result = registry.Remove(handle1);
    EXPECT_TRUE(result);
    EXPECT_FALSE(registry.Contains(handle1));
    EXPECT_EQ(registry.Get(handle1), nullptr);
}

TEST(Registry_unit_tests, Remove_FromBack_UpdatesSparseSet)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Emplace(handle2);
    registry.Emplace(handle3);
    auto result = registry.Remove(handle3);
    EXPECT_TRUE(result);
    EXPECT_EQ(registry.Contains(handle3), false);
    EXPECT_EQ(registry.Get(handle3), nullptr);
}

TEST(Registry_unit_tests, Remove_HandleExceedsCapacity_Throws)
{
    Registry<Fake> registry{1u};
    EXPECT_THROW(registry.Remove(handle2), std::out_of_range);
}

TEST(Registry_unit_tests, Get_ValidHandle_ReturnsPointer)
{
    Registry<Fake> registry{5u};
    auto expected = registry.Emplace(handle1);
    auto actual = registry.Get(handle1);
    EXPECT_EQ(actual, expected);
}

TEST(Registry_unit_tests, Get_InvalidHandle_ReturnsNull)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    auto actual = registry.Get(handle2);
    EXPECT_EQ(actual, nullptr);
}

TEST(Registry_unit_tests, Get_CallAfterRemove_ReturnsNull)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Emplace(handle2);
    auto result = registry.Remove(handle1);
    EXPECT_TRUE(result);
    auto actual = registry.Get(handle1);
    EXPECT_EQ(actual, nullptr);
}

TEST(Registry_unit_tests, Get_CallAfterClear_ReturnsNull)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Emplace(handle2);
    registry.Clear();
    auto actual = registry.Get(handle1);
    EXPECT_EQ(actual, nullptr);
}

TEST(Registry_unit_tests, Get_HandleExceedsCapacity_Throws)
{
    Registry<Fake> registry{1u};
    EXPECT_THROW(registry.Get(handle2), std::out_of_range);
}

TEST(Registry_unit_tests, Contains_ValidHandle_ReturnsTrue)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    auto actual = registry.Contains(handle1);
    EXPECT_TRUE(actual);
}

TEST(Registry_unit_tests, Contains_InvalidHandle_ReturnsFalse)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    auto actual = registry.Contains(handle2);
    EXPECT_FALSE(actual);
}

TEST(Registry_unit_tests, Contains_CallAfterRemove_ReturnsFalse)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    auto removed = registry.Remove(handle1);
    EXPECT_TRUE(removed);
    auto actual = registry.Contains(handle1);
    EXPECT_FALSE(actual);
}

TEST(Registry_unit_tests, Contains_CallAfterClear_ReturnsFalse)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    registry.Clear();
    auto actual = registry.Contains(handle1);
    EXPECT_FALSE(actual);
}

TEST(Registry_unit_tests, Contains_HandleExceedsCapacity_Throws)
{
    Registry<Fake> registry{1u};
    EXPECT_THROW(registry.Contains(handle2), std::out_of_range);
}

TEST(Registry_unit_tests, View_CallAfterClear_ReturnsUpdatedCollection)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle1);
    EXPECT_EQ(registry.View().size(), 1u);
    registry.Clear();
    EXPECT_EQ(registry.View().size(), 0u);
}

TEST(Registry_unit_tests, Clear_FullCollection_RemovesItems)
{
    Registry<Fake> registry{5u};
    registry.Emplace(handle0);
    registry.Emplace(handle1);
    registry.Emplace(handle2);
    registry.Emplace(handle3);
    registry.Emplace(handle4);
    EXPECT_EQ(registry.View().size(), 5u);
    registry.Clear();
    EXPECT_EQ(registry.View().size(), 0u);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}