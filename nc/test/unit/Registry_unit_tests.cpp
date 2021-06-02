#include "gtest/gtest.h"
#include "component/AutoComponentGroup.h"
#include "component/Registry.h"
#include "component/Tag.h"
#include "component/Transform.h"

using namespace nc;
using namespace nc::ecs;

namespace nc
{
    AutoComponentGroup::AutoComponentGroup(Entity)
    {}

    void AutoComponentGroup::SendOnDestroy() {}

    Transform::Transform(Entity entity, const Vector3&, const Quaternion&, const Vector3&, Entity parent)
        : ComponentBase{entity}, m_localMatrix{}, m_worldMatrix{}, m_parent{parent}, m_children{}
    {}

    Quaternion::Quaternion(float X, float Y, float Z, float W)
        : x{X}, y{Y}, z{Z}, w{W}
    {}
}

struct Fake1 : public ComponentBase
{
    Fake1(Entity entity, int v)
        : ComponentBase{entity},
          value{v}
    {}

    int value;
};

struct Fake2 : public ComponentBase
{
    Fake2(Entity entity)
        : ComponentBase{entity}
    {}
};

constexpr auto Handle1 = Entity{EntityUtils::Join(0u, 0u, 0u, 0u)};
constexpr auto Handle2 = Entity{EntityUtils::Join(1u, 0u, 0u, 0u)};
constexpr auto Handle3 = Entity{EntityUtils::Join(2u, 0u, 0u, 0u)};
constexpr auto Handle4 = Entity{EntityUtils::Join(3u, 0u, 0u, 0u)};
constexpr auto Handle5 = Entity{EntityUtils::Join(4u, 0u, 0u, 0u)};
const auto TestInfo = EntityInfo{};

class Registry_unit_tests : public ::testing::Test
{
    public:
        size_t maxEntities = 5u;
        Registry<Transform, AutoComponentGroup, Tag, Fake1, Fake2> registry;

        Registry_unit_tests()
            : registry{maxEntities}
        {}

        ~Registry_unit_tests()
        {
            registry.Clear();
        }
};

TEST_F(Registry_unit_tests, AddEntity_ReturnsNonNull)
{
    auto handle = registry.Add<Entity>(TestInfo);
    EXPECT_TRUE(handle.Valid());
}

TEST_F(Registry_unit_tests, AddEntity_ConsecutiveCalls_ReturnsDistinctHandles)
{
    auto handle1 = registry.Add<Entity>(TestInfo);
    auto handle2 = registry.Add<Entity>(TestInfo);
    EXPECT_NE(handle1, handle2);
}

TEST_F(Registry_unit_tests, AddEntity_AddsTransform)
{
    auto handle = registry.Add<Entity>(TestInfo);
    EXPECT_TRUE(registry.Contains<Transform>(handle));
}

TEST_F(Registry_unit_tests, RemoveEntity_EntityExists_EntityRemovedFromActiveList)
{
    auto handle = registry.Add<Entity>(TestInfo);
    registry.CommitStagedChanges();
    auto entities = registry.ViewAll<Entity>();
    EXPECT_EQ(entities.size(), 1u);
    registry.Remove<Entity>(handle);
    entities = registry.ViewAll<Entity>();
    EXPECT_EQ(entities.size(), 0u);
}

TEST_F(Registry_unit_tests, RemoveEntity_DoesNotExist_Throws)
{
    EXPECT_THROW(registry.Remove<Entity>(Handle1), std::runtime_error);
}

TEST_F(Registry_unit_tests, ContainsEntity_Exists_ReturnsTrue)
{
    auto handle = registry.Add<Entity>(TestInfo);
    EXPECT_TRUE(registry.Contains<Entity>(handle));
}

TEST_F(Registry_unit_tests, ContainsEntity_DoesNotExist_ReturnsFalse)
{
    EXPECT_FALSE(registry.Contains<Entity>(Handle1));
}

TEST_F(Registry_unit_tests, ContainsEntity_AfterRemove_ReturnsFalse)
{
    auto handle = registry.Add<Entity>(TestInfo);
    registry.CommitStagedChanges();
    registry.Remove<Entity>(handle);
    EXPECT_FALSE(registry.Contains<Entity>(handle));
}

TEST_F(Registry_unit_tests, Add_ValidCall_ConstructsObject)
{
    auto* ptr = registry.Add<Fake1>(Handle1, 1);
    EXPECT_EQ(ptr->GetParentEntity(), Handle1);
    EXPECT_EQ(ptr->value, 1);
}

TEST_F(Registry_unit_tests, Add_ReplaceAfterRemove_ConstructsObject)
{
    registry.Add<Fake1>(Handle1, 1);
    registry.Remove<Fake1>(Handle1);
    auto* ptr = registry.Add<Fake1>(Handle1, 2);
    EXPECT_EQ(ptr->GetParentEntity(), Handle1);
    EXPECT_EQ(ptr->value, 2);
}

TEST_F(Registry_unit_tests, Add_DoubleCall_Throws)
{
    registry.Add<Fake1>(Handle1, 1);
    EXPECT_THROW(registry.Add<Fake1>(Handle1, 1), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveComponent_ComponentExists_ReturnsTrue)
{
    registry.Add<Fake1>(Handle1, 1);
    auto actual = registry.Remove<Fake1>(Handle1);
    EXPECT_TRUE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_EmptyRegistry_ReturnsFalse)
{
    auto actual = registry.Remove<Fake1>(Handle1);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_ComponentDoesNotExist_ReturnsFalse)
{
    registry.Add<Fake1>(Handle1, 1);
    auto actual = registry.Remove<Fake1>(Handle2);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_DoubleCall_ReturnsFalse)
{
    registry.Add<Fake1>(Handle1, 1);
    registry.Remove<Fake1>(Handle1);
    auto actual = registry.Remove<Fake1>(Handle1);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_AfterClear_ReturnsFalse)
{
    registry.Add<Fake1>(Handle1, 1);
    registry.Clear();
    auto actual = registry.Remove<Fake1>(Handle1);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_IncorrectType_ReturnsFalse)
{
    registry.Add<Fake1>(Handle1, 1);
    auto actual = registry.Remove<Fake2>(Handle1);
    EXPECT_FALSE(actual);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}