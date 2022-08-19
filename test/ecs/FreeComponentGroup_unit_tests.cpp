#include "gtest/gtest.h"
#include "ecs/detail/FreeComponentGroup.h"

using namespace nc;
using namespace nc::ecs::detail;

struct FakeComponent : public FreeComponent
{
    FakeComponent(Entity entity)
        : FreeComponent{entity}
    {
    }
};

constexpr auto TestEntity = Entity{0, 0, 0};

TEST(FreeComponentGroup_unit_test, Add_ValidCall_ConstructsObject)
{
    auto group = FreeComponentGroup{TestEntity};
    auto ptr = group.Add<FakeComponent>(TestEntity);
    EXPECT_EQ(ptr->ParentEntity(), TestEntity);
}

TEST(FreeComponentGroup_unit_test, Add_ReplaceAfterRemove_ConstructsObject)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeComponent>();
    group.CommitStagedComponents();
    auto ptr = group.Add<FakeComponent>(TestEntity);
    EXPECT_EQ(ptr->ParentEntity(), TestEntity);
}

TEST(FreeComponentGroup_unit_test, Add_DoubleCall_Throws)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    EXPECT_THROW(group.Add<FakeComponent>(TestEntity), std::runtime_error);
}

TEST(FreeComponentGroup_unit_test, Remove_ComponentExists_Removes)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeComponent>();
    group.CommitStagedComponents();
    auto actual = group.Contains<FakeComponent>();
    EXPECT_FALSE(actual);
}

TEST(FreeComponentGroup_unit_test, Remove_ComponentDoesNotExist_Throws)
{
    auto group = FreeComponentGroup{TestEntity};
    EXPECT_THROW(group.Remove<FakeComponent>(), std::runtime_error);
}

TEST(FreeComponentGroup_unit_test, Remove_DoubleCall_Throws)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeComponent>();
    group.CommitStagedComponents();
    EXPECT_THROW(group.Remove<FakeComponent>(), std::runtime_error);
}

TEST(FreeComponentGroup_unit_test, Contains_Exists_ReturnsTrue)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    auto actual = group.Contains<FakeComponent>();
    EXPECT_TRUE(actual);
}

TEST(FreeComponentGroup_unit_test, Contains_ExistsStaged_ReturnsTrue)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    auto actual = group.Contains<FakeComponent>();
    EXPECT_TRUE(actual);
}

TEST(FreeComponentGroup_unit_test, Contains_DoesNotExist_ReturnsFalse)
{
    auto group = FreeComponentGroup{TestEntity};
    auto actual = group.Contains<FakeComponent>();
    EXPECT_FALSE(actual);
}

TEST(FreeComponentGroup_unit_test, Contains_AfterRemoved_ReturnsFalse)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeComponent>();
    group.CommitStagedComponents();
    auto actual = group.Contains<FakeComponent>();
    EXPECT_FALSE(actual);
}

TEST(FreeComponentGroup_unit_test, Get_Exists_ReturnsPointer)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    auto* ptr = group.Get<FakeComponent>();
    EXPECT_NE(ptr, nullptr);
}

TEST(FreeComponentGroup_unit_test, Get_ExistsStaged_ReturnsPointer)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    auto* ptr = group.Get<FakeComponent>();
    EXPECT_NE(ptr, nullptr);
}

TEST(FreeComponentGroup_unit_test, Get_DoesNotExist_ReturnsNull)
{
    auto group = FreeComponentGroup{TestEntity};
    auto* ptr = group.Get<FakeComponent>();
    EXPECT_EQ(ptr, nullptr);
}

TEST(FreeComponentGroup_unit_test, Get_CallAfterRemoved_ReturnsNull)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeComponent>();
    group.CommitStagedComponents();
    auto* ptr = group.Get<FakeComponent>();
    EXPECT_EQ(ptr, nullptr);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}