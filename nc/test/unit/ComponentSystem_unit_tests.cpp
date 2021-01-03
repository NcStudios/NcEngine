#include "gtest/gtest.h"
#include "ecs/ComponentSystem.h"
#include "component/Component.h"

using namespace nc;
using namespace nc::ecs;

EntityHandle TestParentHandle1 = 1u;
EntityHandle TestParentHandle2 = 2u;
EntityHandle TestParentHandle3 = 3u;
int TestVal1 = 5;
int TestVal2 = 6;
int TestVal3 = 7;
EntityHandle TestBadHandle = 10u;

struct Fake : public Component
{
    Fake(ComponentHandle handle, EntityHandle parentHandle, int in)
        : Component(handle, parentHandle),
          val{in}
    {}

    int val;
};

TEST(ComponentSystem_unit_tests, Add_FirstAlloc_ReturnsNonNullHandle)
{
    ComponentSystem<Fake> system;
    auto actual = system.Add(TestParentHandle1, TestVal1);
    EXPECT_NE(actual, NullHandle);
}

TEST(ComponentSystem_unit_tests, Add_PoolFull_AllocatesNewPool)
{
    ComponentSystem<Fake> system(1u, false);
    auto firstHandle = system.Add(TestParentHandle1, TestVal1);
    auto actual = system.Add(TestBadHandle, TestVal1);
    EXPECT_EQ(actual, firstHandle + 1);
}

TEST(ComponentSystem_unit_tests, Add_PoolFullWithIsReserveSizeMaxSizeTrue_Throws)
{
    ComponentSystem<Fake> system(1u, true);
    system.Add(TestParentHandle1, TestVal1);
    EXPECT_THROW(system.Add(TestBadHandle, TestVal1), std::runtime_error);
}

TEST(ComponentSystem_unit_tests, Contains_GoodHandle_ReturnsTrue)
{
    ComponentSystem<Fake> system;
    auto handle = system.Add(TestParentHandle1, TestVal1);
    auto actual = system.Contains(handle);
    EXPECT_EQ(actual, true);
}

TEST(ComponentSystem_unit_tests, Contains_BadHandle_ReturnsFalse)
{
    ComponentSystem<Fake> system;
    system.Add(TestParentHandle1, TestVal1);
    auto actual = system.Contains(TestBadHandle);
    EXPECT_EQ(actual, false);
}

TEST(ComponentSystem_unit_tests, Contains_Empty_ReturnsFalse)
{
    ComponentSystem<Fake> system;
    auto actual = system.Contains(TestBadHandle);
    EXPECT_EQ(actual, false);
}

TEST(ComponentSystem_unit_tests, Contains_AfterClear_ReturnsFalse)
{
    ComponentSystem<Fake> system;
    auto handle = system.Add(TestParentHandle1, TestVal1);
    system.Clear();
    auto actual = system.Contains(handle);
    EXPECT_EQ(actual, false);
}

TEST(ComponentSystem_unit_tests, Remove_GoodHandle_ReturnsTrue)
{
    ComponentSystem<Fake> system;
    auto handle = system.Add(TestParentHandle1, TestVal1);
    auto actual = system.Remove(handle);
    EXPECT_EQ(actual, true);
}

TEST(ComponentSystem_unit_tests, Remove_BadHandle_ReturnsFalse)
{
    ComponentSystem<Fake> system;
    system.Add(TestParentHandle1, TestVal1);
    auto actual = system.Remove(TestBadHandle);
    EXPECT_EQ(actual, false);
}

TEST(ComponentSystem_unit_tests, Remove_Empty_ReturnsFalse)
{
    ComponentSystem<Fake> system;
    auto actual = system.Remove(TestBadHandle);
    EXPECT_EQ(actual, false);
}

TEST(ComponentSystem_unit_tests, Remove_AfterClear_ReturnsFalse)
{
    ComponentSystem<Fake> system;
    auto handle = system.Add(TestParentHandle1, TestVal1);
    system.Clear();
    auto actual = system.Remove(handle);
    EXPECT_EQ(actual, false);
}

TEST(ComponentSystem_unit_tests, GetPointerTo_GoodHandle_ReturnsNull)
{
    ComponentSystem<Fake> system;
    auto handle = system.Add(TestParentHandle1, TestVal1);
    auto actual = system.GetPointerTo(handle);
    EXPECT_EQ(actual->val, TestVal1);
}

TEST(ComponentSystem_unit_tests, GetPointerTo_BadHandle_ReturnsNull)
{
    ComponentSystem<Fake> system;
    system.Add(TestParentHandle1, TestVal1);
    auto actual = system.GetPointerTo(TestBadHandle);
    EXPECT_EQ(actual, nullptr);
}

TEST(ComponentSystem_unit_tests, GetPointerTo_Empty_ReturnsNull)
{
    ComponentSystem<Fake> system;
    auto actual = system.GetPointerTo(TestBadHandle);
    EXPECT_EQ(actual, nullptr);
}

TEST(ComponentSystem_unit_tests, GetPointerTo_AfterClear_ReturnsNull)
{
    ComponentSystem<Fake> system;
    auto handle = system.Add(TestParentHandle1, TestVal1);
    system.Clear();
    auto actual = system.GetPointerTo(handle);
    EXPECT_EQ(actual, nullptr);
}

TEST(ComponentSystem_unit_tests, ForEach_ContiguousComponents_VisitsEach)
{
    ComponentSystem<Fake> system;
    system.Add(TestParentHandle1, TestVal1);
    system.Add(TestParentHandle2, TestVal2);
    system.Add(TestParentHandle3, TestVal3);
    auto actual = 0u;
    system.ForEach([&actual](auto& element)
    {
        actual += element.val;
    });
    auto expected = TestVal1 + TestVal2 + TestVal3;
    EXPECT_EQ(actual, expected);
}

TEST(ComponentSystem_unit_tests, ForEach_NonContiguousComponents_SkipsGap)
{
    ComponentSystem<Fake> system;
    system.Add(TestParentHandle1, TestVal1);
    auto toRemove = system.Add(TestParentHandle2, TestVal2);
    system.Add(TestParentHandle3, TestVal3);
    system.Remove(toRemove);
    auto actual = 0u;
    system.ForEach([&actual](auto& element)
    {
        actual += element.val;
    });
    auto expected = TestVal1 + TestVal3;
    EXPECT_EQ(actual, expected);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}