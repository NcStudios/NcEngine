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
    auto& component = group.Add<FakeComponent>(TestEntity);
    EXPECT_EQ(component.ParentEntity(), TestEntity);
}

TEST(FreeComponentGroup_unit_test, Add_ReplaceAfterRemove_ConstructsObject)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeComponent>();
    group.CommitStagedComponents();
    auto& component = group.Add<FakeComponent>(TestEntity);
    EXPECT_EQ(component.ParentEntity(), TestEntity);
}

TEST(FreeComponentGroup_unit_test, Add_DoubleCall_Throws)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    EXPECT_THROW(group.Add<FakeComponent>(TestEntity), std::runtime_error);
}

TEST(FreeComponentGroup_unit_test, Remove_ComponentExists_ReturnsTrue)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    EXPECT_TRUE(group.Remove<FakeComponent>());
    group.CommitStagedComponents();
    auto actual = group.Contains<FakeComponent>();
    EXPECT_FALSE(actual);
}

TEST(FreeComponentGroup_unit_test, Remove_ComponentDoesNotExist_ReturnsFalse)
{
    auto group = FreeComponentGroup{TestEntity};
    EXPECT_FALSE(group.Remove<FakeComponent>());
}

TEST(FreeComponentGroup_unit_test, Remove_DoubleCall_ReturnsFalse)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeComponent>();
    group.CommitStagedComponents();
    EXPECT_FALSE(group.Remove<FakeComponent>());
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

TEST(FreeComponentGroup_unit_test, Get_Exists_Succeeds)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    EXPECT_NO_THROW(group.Get<FakeComponent>());
}

TEST(FreeComponentGroup_unit_test, Get_ExistsStaged_Succeeds)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    EXPECT_NO_THROW(group.Get<FakeComponent>());
}

TEST(FreeComponentGroup_unit_test, Get_DoesNotExist_Throws)
{
    auto group = FreeComponentGroup{TestEntity};
    EXPECT_THROW(group.Get<FakeComponent>(), nc::NcError);
}

TEST(FreeComponentGroup_unit_test, Get_CallAfterRemoved_ReturnsNull)
{
    auto group = FreeComponentGroup{TestEntity};
    group.Add<FakeComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeComponent>();
    group.CommitStagedComponents();
    EXPECT_THROW(group.Get<FakeComponent>(), nc::NcError);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
