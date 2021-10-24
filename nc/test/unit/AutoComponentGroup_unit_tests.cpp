#include "gtest/gtest.h"
#include "ecs/component/AutoComponentGroup.h"

using namespace nc;

struct FakeAutoComponent : public AutoComponent
{
    FakeAutoComponent(Entity entity)
        : AutoComponent{entity}
    {
    }
};

constexpr auto TestEntity = Entity{0, 0, 0};

TEST(AutoComponentGroup_unit_tests, Add_ValidCall_ConstructsObject)
{
    auto group = AutoComponentGroup{TestEntity};
    auto ptr = group.Add<FakeAutoComponent>(TestEntity);
    EXPECT_EQ(ptr->GetParentEntity(), TestEntity);
}

TEST(AutoComponentGroup_unit_tests, Add_ReplaceAfterRemove_ConstructsObject)
{
    auto group = AutoComponentGroup{TestEntity};
    group.Add<FakeAutoComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeAutoComponent>();
    group.CommitStagedComponents();
    auto ptr = group.Add<FakeAutoComponent>(TestEntity);
    EXPECT_EQ(ptr->GetParentEntity(), TestEntity);
}

TEST(AutoComponentGroup_unit_tests, Add_DoubleCall_Throws)
{
    auto group = AutoComponentGroup{TestEntity};
    group.Add<FakeAutoComponent>(TestEntity);
    EXPECT_THROW(group.Add<FakeAutoComponent>(TestEntity), std::runtime_error);
}

TEST(AutoComponentGroup_unit_tests, Remove_ComponentExists_Removes)
{
    auto group = AutoComponentGroup{TestEntity};
    group.Add<FakeAutoComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeAutoComponent>();
    group.CommitStagedComponents();
    auto actual = group.Contains<FakeAutoComponent>();
    EXPECT_FALSE(actual);
}

TEST(AutoComponentGroup_unit_tests, Remove_ComponentDoesNotExist_Throws)
{
    auto group = AutoComponentGroup{TestEntity};
    EXPECT_THROW(group.Remove<FakeAutoComponent>(), std::runtime_error);
}

TEST(AutoComponentGroup_unit_tests, Remove_DoubleCall_Throws)
{
    auto group = AutoComponentGroup{TestEntity};
    group.Add<FakeAutoComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeAutoComponent>();
    group.CommitStagedComponents();
    EXPECT_THROW(group.Remove<FakeAutoComponent>(), std::runtime_error);
}

TEST(AutoComponentGroup_unit_tests, Contains_Exists_ReturnsTrue)
{
    auto group = AutoComponentGroup{TestEntity};
    group.Add<FakeAutoComponent>(TestEntity);
    group.CommitStagedComponents();
    auto actual = group.Contains<FakeAutoComponent>();
    EXPECT_TRUE(actual);
}

TEST(AutoComponentGroup_unit_tests, Contains_ExistsStaged_ReturnsTrue)
{
    auto group = AutoComponentGroup{TestEntity};
    group.Add<FakeAutoComponent>(TestEntity);
    auto actual = group.Contains<FakeAutoComponent>();
    EXPECT_TRUE(actual);
}

TEST(AutoComponentGroup_unit_tests, Contains_DoesNotExist_ReturnsFalse)
{
    auto group = AutoComponentGroup{TestEntity};
    auto actual = group.Contains<FakeAutoComponent>();
    EXPECT_FALSE(actual);
}

TEST(AutoComponentGroup_unit_tests, Contains_AfterRemoved_ReturnsFalse)
{
    auto group = AutoComponentGroup{TestEntity};
    group.Add<FakeAutoComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeAutoComponent>();
    group.CommitStagedComponents();
    auto actual = group.Contains<FakeAutoComponent>();
    EXPECT_FALSE(actual);
}

TEST(AutoComponentGroup_unit_tests, Get_Exists_ReturnsPointer)
{
    auto group = AutoComponentGroup{TestEntity};
    group.Add<FakeAutoComponent>(TestEntity);
    group.CommitStagedComponents();
    auto* ptr = group.Get<FakeAutoComponent>();
    EXPECT_NE(ptr, nullptr);
}

TEST(AutoComponentGroup_unit_tests, Get_ExistsStaged_ReturnsPointer)
{
    auto group = AutoComponentGroup{TestEntity};
    group.Add<FakeAutoComponent>(TestEntity);
    auto* ptr = group.Get<FakeAutoComponent>();
    EXPECT_NE(ptr, nullptr);
}

TEST(AutoComponentGroup_unit_tests, Get_DoesNotExist_ReturnsNull)
{
    auto group = AutoComponentGroup{TestEntity};
    auto* ptr = group.Get<FakeAutoComponent>();
    EXPECT_EQ(ptr, nullptr);
}

TEST(AutoComponentGroup_unit_tests, Get_CallAfterRemoved_ReturnsNull)
{
    auto group = AutoComponentGroup{TestEntity};
    group.Add<FakeAutoComponent>(TestEntity);
    group.CommitStagedComponents();
    group.Remove<FakeAutoComponent>();
    group.CommitStagedComponents();
    auto* ptr = group.Get<FakeAutoComponent>();
    EXPECT_EQ(ptr, nullptr);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}