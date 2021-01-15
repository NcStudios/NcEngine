#include "gtest/gtest.h"
#include "ecs/Entity.h"

using namespace nc;

struct Mock : Component
{
    Mock() = default;
    Mock(EntityHandle handle)
        : Component(handle)
    {}
};

struct Mock2 : Component
{
    Mock2() = default;
    Mock2(EntityHandle handle)
        : Component(handle)
    {}
};

TEST(Entity_unit_tests, AddUserComponent_ReturnsNonNull)
{
    auto entity = Entity(1);
    auto actual = entity.AddUserComponent<Mock>();
    EXPECT_NE(actual, nullptr);
}

TEST(Entity_unit_tests, AddUserComponent_DuplicateComponent_ReturnsNull)
{
    auto entity = Entity(1);
    entity.AddUserComponent<Mock>();
    auto actual = entity.AddUserComponent<Mock>();
    EXPECT_EQ(actual, nullptr);
}

TEST(Entity_unit_tests, GetUserComponent_HasComponent_ReturnsNonNull)
{
    auto entity = Entity(1);
    entity.AddUserComponent<Mock>();
    auto actual = entity.GetUserComponent<Mock>();
    EXPECT_NE(actual, nullptr);
}

TEST(Entity_unit_tests, GetUserComponent_DoesNotHaveComponent_ReturnsNull)
{
    auto entity = Entity(1);
    auto actual = entity.GetUserComponent<Mock>();
    EXPECT_EQ(actual, nullptr);
}

TEST(Entity_unit_tests, HasUserComponent_HasComponent_ReturnsTrue)
{
    auto entity = Entity(1);
    entity.AddUserComponent<Mock>();
    auto actual = entity.HasUserComponent<Mock>();
    EXPECT_EQ(actual, true);
}

TEST(Entity_unit_tests, HasUserComponent_DoesNotHaveComponent_ReturnsFalse)
{
    auto entity = Entity(1);
    auto actual = entity.HasUserComponent<Mock>();
    EXPECT_EQ(actual, false);
}

TEST(Entity_unit_tests, RemoveUserComponent_HasComponent_ReturnsTrue)
{
    auto entity = Entity(1);
    entity.AddUserComponent<Mock>();
    auto actual = entity.RemoveUserComponent<Mock>();
    EXPECT_EQ(actual, true);
}

TEST(Entity_unit_tests, RemoveUserComponent_DoesNotHaveComponent_ReturnsFalse)
{
    auto entity = Entity(1);
    auto actual = entity.RemoveUserComponent<Mock>();
    EXPECT_EQ(actual, false);
}

TEST(Entity_unit_tests, GetUserComponents_NonEmptyCollection_HasCorrectCount)
{
    auto entity = Entity(1);
    entity.AddUserComponent<Mock>();
    entity.AddUserComponent<Mock2>();
    const auto& vec = entity.GetUserComponents();
    auto actual = vec.size();
    EXPECT_EQ(actual, 2u);
}

TEST(Entity_unit_tests, GetUserComponents_AllComponentsRemoved_IsEmpty)
{
    auto entity = Entity(1);
    entity.AddUserComponent<Mock>();
    entity.AddUserComponent<Mock2>();
    entity.RemoveUserComponent<Mock>();
    entity.RemoveUserComponent<Mock2>();
    const auto& vec = entity.GetUserComponents();
    auto actual = vec.size();
    EXPECT_EQ(actual, 0u);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}