#include "gtest/gtest.h"
#include "component/Component.h"
#include "ecs/Registry.h"

using namespace nc;
using namespace nc::ecs;

struct Fake1 : public Component
{
    Fake1(EntityHandle handle, int v)
        : Component{handle},
          value{v}
    {}

    int value;
};

struct Fake2 : public Component
{
    Fake2(EntityHandle handle)
        : Component{handle}
    {}
};

constexpr auto Handle1 = EntityHandle{0u, 0u, 0u, 0u};
constexpr auto Handle2 = EntityHandle{1u, 0u, 0u, 0u};
constexpr auto Handle3 = EntityHandle{2u, 0u, 0u, 0u};
constexpr auto Handle4 = EntityHandle{3u, 0u, 0u, 0u};
constexpr auto Handle5 = EntityHandle{4u, 0u, 0u, 0u};

class Registry_unit_tests : public ::testing::Test
{
    public:
        size_t maxEntities = 5u;
        Registry<Fake1, Fake2> registry;

        Registry_unit_tests()
            : registry{maxEntities}
        {}

        ~Registry_unit_tests()
        {
            registry.Clear();
        }
};

TEST_F(Registry_unit_tests, AddComponent_ValidCall_ConstructsObject)
{
    auto* ptr = registry.AddComponent<Fake1>(Handle1, 1);
    EXPECT_EQ(ptr->GetParentHandle(), Handle1);
    EXPECT_EQ(ptr->value, 1);
}

TEST_F(Registry_unit_tests, AddComponent_ReplaceAfterRemove_ConstructsObject)
{
    registry.AddComponent<Fake1>(Handle1, 1);
    registry.RemoveComponent<Fake1>(Handle1);
    auto* ptr = registry.AddComponent<Fake1>(Handle1, 2);
    EXPECT_EQ(ptr->GetParentHandle(), Handle1);
    EXPECT_EQ(ptr->value, 2);
}

TEST_F(Registry_unit_tests, AddComponent_DoubleCall_Throws)
{
    registry.AddComponent<Fake1>(Handle1, 1);
    EXPECT_THROW(registry.AddComponent<Fake1>(Handle1, 1), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveComponent_ComponentExists_ReturnsTrue)
{
    registry.AddComponent<Fake1>(Handle1, 1);
    auto actual = registry.RemoveComponent<Fake1>(Handle1);
    EXPECT_TRUE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_EmptyRegistry_ReturnsFalse)
{
    auto actual = registry.RemoveComponent<Fake1>(Handle1);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_ComponentDoesNotExist_ReturnsFalse)
{
    registry.AddComponent<Fake1>(Handle1, 1u);
    auto actual = registry.RemoveComponent<Fake1>(Handle2);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_DoubleCall_ReturnsFalse)
{
    registry.AddComponent<Fake1>(Handle1, 1u);
    registry.RemoveComponent<Fake1>(Handle1);
    auto actual = registry.RemoveComponent<Fake1>(Handle1);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_AfterClear_ReturnsFalse)
{
    registry.AddComponent<Fake1>(Handle1, 1u);
    registry.Clear();
    auto actual = registry.RemoveComponent<Fake1>(Handle1);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_IncorrectType_ReturnsFalse)
{
    registry.AddComponent<Fake1>(Handle1, 1);
    auto actual = registry.RemoveComponent<Fake2>(Handle1);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, random)
{
    EXPECT_EQ(registry.ViewAll<Fake1>().size(), 0u);
    EXPECT_EQ(registry.ViewAll<Fake2>().size(), 0u);

    registry.AddComponent<Fake1>(Handle1, 1);
    registry.AddComponent<Fake1>(Handle2, 2);
    registry.AddComponent<Fake1>(Handle3, 3);

    EXPECT_EQ(registry.HasComponent<Fake1>(Handle1), true);
    EXPECT_EQ(registry.HasComponent<Fake1>(Handle2), true);
    EXPECT_EQ(registry.HasComponent<Fake1>(Handle3), true);

    EXPECT_EQ(registry.ViewAll<Fake1>().size(), 3u);
    EXPECT_EQ(registry.ViewAll<Fake2>().size(), 0u);

    registry.AddComponent<Fake2>(Handle2);
    auto removed = registry.RemoveComponent<Fake1>(Handle2);
    EXPECT_TRUE(removed);

    EXPECT_EQ(registry.HasComponent<Fake1>(Handle1), true);
    EXPECT_EQ(registry.HasComponent<Fake1>(Handle2), false);
    EXPECT_EQ(registry.HasComponent<Fake1>(Handle3), true);

    EXPECT_EQ(registry.HasComponent<Fake2>(Handle1), false);
    EXPECT_EQ(registry.HasComponent<Fake2>(Handle2), true);
    EXPECT_EQ(registry.HasComponent<Fake2>(Handle3), false);

    EXPECT_EQ(registry.ViewAll<Fake1>().size(), 2u);
    EXPECT_EQ(registry.ViewAll<Fake2>().size(), 1u);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}