#include "gtest/gtest.h"
#include "ecs/ComponentSystem.h"
#include "component/Component.h"

using namespace nc;
using namespace nc::ecs;

struct Fake : public ComponentBase
{
    Fake(EntityHandle handle, int in)
        : ComponentBase(handle),
          val{in}
    {}

    int val;
};

constexpr auto TestLayer = HandleTraits::layer_type{0u};
constexpr auto TestVersion = HandleTraits::version_type{0u};
constexpr auto TestFlags = EntityFlags::None;

EntityHandle TestParentHandle1{1u, TestVersion, TestLayer, TestFlags};
EntityHandle TestParentHandle2{2u, TestVersion, TestLayer, TestFlags};
EntityHandle TestParentHandle3{3u, TestVersion, TestLayer, TestFlags};
EntityHandle TestParentHandle4{4u, TestVersion, TestLayer, TestFlags};
EntityHandle TestParentHandle5{5u, TestVersion, TestLayer, TestFlags};
EntityHandle TestBadHandle{10u, TestVersion, TestLayer, TestFlags};
int TestVal1 = 20;
int TestVal2 = 21;
int TestVal3 = 22;
int TestVal4 = 23;
int TestVal5 = 24;

class ComponentSystem_unit_tests : public ::testing::Test
{
    public:
        ComponentSystem_unit_tests()
            : fakeSystem{4u}
        {
        }

        ~ComponentSystem_unit_tests()
        {
            fakeSystem.Clear();
        }

        ComponentSystem<Fake> fakeSystem;
};

TEST_F(ComponentSystem_unit_tests, Add_FirstAlloc_ReturnsNonNull)
{
    auto actual = fakeSystem.Add(TestParentHandle1, TestVal1);
    EXPECT_NE(actual, nullptr);
    fakeSystem.Remove(TestParentHandle1);
}

TEST_F(ComponentSystem_unit_tests, Add_ConsecutiveCallsOnEmptySystem_ReturnsSequentialAddresses)
{
    auto first = fakeSystem.Add(TestParentHandle1, TestVal1);
    auto second = fakeSystem.Add(TestParentHandle2, TestVal2);
    auto actual = second - first;
    EXPECT_EQ(actual, 1u);
}

TEST_F(ComponentSystem_unit_tests, Add_SystemFull_Throws)
{
    fakeSystem.Add(TestParentHandle1, TestVal1);
    fakeSystem.Add(TestParentHandle2, TestVal2);
    fakeSystem.Add(TestParentHandle3, TestVal3);
    fakeSystem.Add(TestParentHandle4, TestVal4);
    EXPECT_THROW(fakeSystem.Add(TestParentHandle5, TestVal5), alloc::MemoryResourceBadAlloc);
}

TEST_F(ComponentSystem_unit_tests, Contains_GoodHandle_ReturnsTrue)
{
    fakeSystem.Add(TestParentHandle1, TestVal1);
    auto actual = fakeSystem.Contains(TestParentHandle1);
    EXPECT_EQ(actual, true);
}

TEST_F(ComponentSystem_unit_tests, Contains_BadHandle_ReturnsFalse)
{
    fakeSystem.Add(TestParentHandle1, TestVal1);
    auto actual = fakeSystem.Contains(TestBadHandle);
    EXPECT_EQ(actual, false);
}

TEST_F(ComponentSystem_unit_tests, Contains_Empty_ReturnsFalse)
{
    auto actual = fakeSystem.Contains(TestBadHandle);
    EXPECT_EQ(actual, false);
}

TEST_F(ComponentSystem_unit_tests, Contains_AfterRemove_ReturnsFalse)
{
    fakeSystem.Add(TestParentHandle1, TestVal1);
    fakeSystem.Remove(TestParentHandle1);
    auto actual = fakeSystem.Contains(TestParentHandle1);
    EXPECT_EQ(actual, false);
}

TEST_F(ComponentSystem_unit_tests, Contains_AfterClear_ReturnsFalse)
{
    fakeSystem.Add(TestParentHandle1, TestVal1);
    fakeSystem.Clear();
    auto actual = fakeSystem.Contains(TestParentHandle1);
    EXPECT_EQ(actual, false);
}

TEST_F(ComponentSystem_unit_tests, Remove_GoodHandle_ReturnsTrue)
{
    fakeSystem.Add(TestParentHandle1, TestVal1);
    auto actual = fakeSystem.Remove(TestParentHandle1);
    EXPECT_EQ(actual, true);
}

TEST_F(ComponentSystem_unit_tests, Remove_BadHandle_ReturnsFalse)
{
    fakeSystem.Add(TestParentHandle1, TestVal1);
    auto actual = fakeSystem.Remove(TestBadHandle);
    EXPECT_EQ(actual, false);
}

TEST_F(ComponentSystem_unit_tests, Remove_Empty_ReturnsFalse)
{
    auto actual = fakeSystem.Remove(TestBadHandle);
    EXPECT_EQ(actual, false);
}

TEST_F(ComponentSystem_unit_tests, Remove_AfterClear_ReturnsFalse)
{
    fakeSystem.Add(TestParentHandle1, TestVal1);
    fakeSystem.Clear();
    auto actual = fakeSystem.Remove(TestParentHandle1);
    EXPECT_EQ(actual, false);
}

TEST_F(ComponentSystem_unit_tests, GetPointerTo_GoodHandle_EqualsPointerFromAdd)
{
    auto expected = fakeSystem.Add(TestParentHandle1, TestVal1);
    auto actual = fakeSystem.GetPointerTo(TestParentHandle1);
    EXPECT_EQ(actual, expected);
}

TEST_F(ComponentSystem_unit_tests, GetPointerTo_BadHandle_ReturnsNull)
{
    fakeSystem.Clear();
    fakeSystem.Add(TestParentHandle1, TestVal1);
    auto actual = fakeSystem.GetPointerTo(TestBadHandle);
    EXPECT_EQ(actual, nullptr);
}

TEST_F(ComponentSystem_unit_tests, GetPointerTo_Empty_ReturnsNull)
{
    auto actual = fakeSystem.GetPointerTo(TestBadHandle);
    EXPECT_EQ(actual, nullptr);
}

TEST_F(ComponentSystem_unit_tests, GetPointerTo_AfterRemove_ReturnsNull)
{
    fakeSystem.Add(TestParentHandle1, TestVal1);
    fakeSystem.Remove(TestParentHandle1);
    auto actual = fakeSystem.GetPointerTo(TestParentHandle1);
    EXPECT_EQ(actual, nullptr);
}

TEST_F(ComponentSystem_unit_tests, GetPointerTo_AfterClear_ReturnsNull)
{
    fakeSystem.Add(TestParentHandle1, TestVal1);
    fakeSystem.Clear();
    auto actual = fakeSystem.GetPointerTo(TestParentHandle1);
    EXPECT_EQ(actual, nullptr);
}

TEST_F(ComponentSystem_unit_tests, GetComponents_SequentiallyAdded_GetsAllComponents)
{
    auto actualAddress1 = fakeSystem.Add(TestParentHandle1, TestVal1);
    auto actualAddress2 = fakeSystem.Add(TestParentHandle2, TestVal2);
    auto actualAddress3 = fakeSystem.Add(TestParentHandle3, TestVal3);

    auto fakes = fakeSystem.GetComponents();
    auto actualSize = fakes.size();
    EXPECT_EQ(actualSize, 3);

    EXPECT_EQ(actualAddress1, fakes[0]);
    EXPECT_EQ(actualAddress2, fakes[1]);
    EXPECT_EQ(actualAddress3, fakes[2]);

    EXPECT_EQ(TestVal1, fakes[0]->val);
    EXPECT_EQ(TestVal2, fakes[1]->val);
    EXPECT_EQ(TestVal3, fakes[2]->val);
}

TEST_F(ComponentSystem_unit_tests, GetComponents_AddThroughFreeList_ReturnsInIncreasingOrder)
{
    auto actualAddress1 = fakeSystem.Add(TestParentHandle1, TestVal1);
    fakeSystem.Add(TestParentHandle2, TestVal2);
    auto actualAddress3 = fakeSystem.Add(TestParentHandle3, TestVal3);
    fakeSystem.Remove(TestParentHandle2);
    auto actualAddress4 = fakeSystem.Add(TestParentHandle4, TestVal4);
    auto fakes = fakeSystem.GetComponents();
    auto actualSize = fakes.size();
    EXPECT_EQ(actualSize, 3);
    EXPECT_EQ(actualAddress1, fakes[0]);
    EXPECT_EQ(actualAddress4, fakes[1]);
    EXPECT_EQ(actualAddress3, fakes[2]);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}